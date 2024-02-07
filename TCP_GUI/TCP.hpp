#pragma once
#include "type_def.hpp"
using error_code = tcp::type::error_code;
using queue = tcp::type::tsqueue;
using state = tcp::type::TCPSessionState;
using HEADER = tcp::type::TCPHeader;
using SEGMENT = tcp::type::TCPSegment;
using session = tcp::TCPSession;
using option = tcp::type::TCPOption;
/// <summary>
/// Maps the IP addresses with the 
/// TO DO: Tidy it up, it's very messy to have it GLOBALIZED. Encapsulate it into a class or something
/// </summary>
std::map<std::string, session> IP_MAP;
std::map<int, session> ID_MAP; // used for checking later for errors it would be deleted when the ACTUAL implementation happens
namespace tcp {
	class TCPSession
	{
	public:
		// TO DO: Further protect this with a getter
		queue incomingSegments;
	
	public:
		error_code getTargetSession()
		{
			
			auto sessionIter = IP_MAP.find(_destinationIp);
			if (sessionIter != IP_MAP.end()) {
				_connectedSession = &sessionIter->second;
				return error_code::no_error;
			}
			else
			{
				return error_code::IP_NOT_FOUND;
			}
		}
		error_code listen()
		{
			this->_currentState = state::LISTEN;
			receiver = std::thread(&TCPSession::receive, this);
		}

		error_code connetct(std::string& ip, SIZE_PORT& port, SIZE_SEQ SEQ, SIZE_ACK ACK, SIZE_WINDOW WIN, std::vector<option> options)
		{
			HEADER synPacket;
			_destinationPort = port;
			_destinationIp = ip;
			synPacket._SEQ = 4879234; // TODO: Randomize this number
			synPacket._ACK = 0; 
			synPacket._window = 1460; // TODO: For now it's STATIC, make it dynamic and calculate the segments forward this time.
		}

		error_code connect(std::string& ip, SIZE_PORT& port, HEADER& synPacket)
		{
			_destinationPort = port;
			_destinationIp = ip;
			// TODO: Check the synPacket if it's correct
			SEGMENT _Syn_Segment(synPacket);
			send(_Syn_Segment);
			this->_currentState = state::SYN_SENT;
		}
		error_code disconnect()
		{
			// Figure out a way to make it batter later
			this->_currentState = state::LISTEN;
		}
		error_code send(SEGMENT segment)
		{
			error_code err = getTargetSession();
			if (err != error_code::no_error)
			{
				return err;
			}
			_connectedSession.incomingSegments.push_back(segment);
		}
		error_code receive(TCP_DATA_TYPE& data )
		{
			while (_currentState == state::LISTEN)
			{
				SEGMENT currentSegment = incomingSegments.pop_front();

				// Extract common header fields for easier access
				HEADER& hdr = currentSegment.header;

				// Handling based on the current session state
				switch (_currentState) {
				case tcp::type::TCPSessionState::LISTEN: {
					// Expecting SYN to initiate a connection
					if (hdr._flags & tcp::type::TCPFlags::SYN) {
						// Transition to SYN_RECEIVED state and send SYN-ACK
						_currentState = tcp::type::TCPSessionState::SYN_RECEIVED;
						sendSynAck(currentSegment);
					}
					break;
				}
				case tcp::type::TCPSessionState::SYN_SENT: {
					// Expecting SYN-ACK in response to our SYN
					if ((hdr._flags & tcp::type::TCPFlags::SYN) && (hdr._flags & tcp::type::TCPFlags::ACK)) {
						// Transition to ESTABLISHED state after receiving SYN-ACK
						_currentState = tcp::type::TCPSessionState::ESTABLISHED;
						sendAck(hdr._destinationPort, hdr._sourcePort, hdr._SEQ);
					}
					break;
				}
				case tcp::type::TCPSessionState::ESTABLISHED: {
					// Handle PSH flag for pushing data
					if (hdr._flags & tcp::type::TCPFlags::PSH) {
						// TODO: Handle the data in a more efficient way this is far too unsafe
						data = currentSegment.data; // here's the data, you do whatever you like with it
						// Optionally, send an ACK back for the received data
						sendAck(hdr._destinationPort, hdr._sourcePort, hdr._SEQ + currentSegment.data.size());
					}
					break;
				}
				default:
					return error_code::HEAEDER_ERROR;
					break;
				}
			}
			return tcp::type::error_code::no_error; // Adjust error handling as needed
		}
		error_code sendAck(uint16_t sourcePort, uint16_t destinationPort, uint32_t nextSeq) {
			tcp::type::TCPHeader header;
			header._sourcePort = sourcePort;
			header._destinationPort = destinationPort;
			header._SEQ = this->_SEQ_; // Use the current SEQ number for this session
			header._ACK = nextSeq; // Set ACK to the next expected sequence number
			header._flags = tcp::type::TCPFlags::ACK; // Set ACK flag
			header._window = this->_WIN_; // Use the current window size for this session
			// Set other header fields as necessary
			
			tcp::type::TCPSegment segment(header);
			// No data payload for a pure ACK, unless piggybacking data

			send(segment); // Dispatch the ACK segment
		}
		error_code sendSynAck(SEGMENT _REC_SEG)
		{
			HEADER header;
			header._sourcePort = _MyPort;
			header._destinationPort = _destinationPort;
			header._SEQ = 4879234; // Assume you have a method to generate a sequence number
			header._ACK = _REC_SEG.header._SEQ + 1; // ACK is the received SEQ + 1
			header._flags = tcp::type::TCPFlags::SYN | tcp::type::TCPFlags::ACK; // Set SYN-ACK flags
			header._window = 8192; // Example window size, set as needed
			// Set other header fields as necessary

			SEGMENT segment(header);

			send(segment); // Dispatch the SYN-ACK segment
		}
		TCPSession() = delete;
		TCPSession(std::string&& ip, SIZE_PORT port) : _MyIp(ip), _MyPort(port) , _SEQ_(4879234), _ACK_(0), _WIN_(1460){}

		~TCPSession()
		{
		}

	private:
		SIZE_SEQ _SEQ_;
		SIZE_ACK _ACK_;
		SIZE_PORT _MyPort;
		SIZE_PORT _destinationPort;
		SIZE_WINDOW _WIN_;
		std::string _MyIp;
		std::string _destinationIp;
		state _currentState = state::CLOSED;
		session _connectedSession = session(std::move("0.0.0.0"), std::move(1));
		std::thread receiver;
	};
}
