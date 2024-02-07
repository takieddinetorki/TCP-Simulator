#pragma once

#pragma region _STD_INCLUDES
	#include <vector>
	#include <memory>
	#include <array>
	#include <thread>
	#include <chrono>
	#include <deque>
	#include <mutex>
	#include <optional>
	#include <map>
#pragma endregion

#pragma region _SIZE_DEFINITIONS
	#define SIZE_PORT uint16_t
	#define SIZE_SEQ uint32_t
	#define SIZE_ACK uint32_t
	#define SIZE_FLAGS uint8_t
	#define SIZE_WINDOW uint16_t
	#define SIZE_CHECKSUM uint16_t
	#define SIZE_URGENT uint16_t
	#define TCP_DATA_TYPE std::vector<uint8_t>
#pragma endregion



namespace tcp {
	namespace type
	{
		typedef enum error {
			no_error = 0x00,
			IP_NOT_FOUND,
			HEAEDER_ERROR,
		} error_code;
		typedef enum TCPSessionState {
			LISTEN,
			SYN_SENT,
			SYN_RECEIVED,
			ESTABLISHED,
			FIN_WAIT_1,
			FIN_WAIT_2,
			CLOSE_WAIT,
			CLOSING,
			LAST_ACK,
			TIME_WAIT,
			CLOSED
		} STATE;
		typedef enum TCPOptionKind {
			MAXIMUM_SEGMENT_SIZE = 0,
			SELECTIVE_ACKNOWLEDGMENTS = 1,
			TIMESTAMPS = 2,
			NO_OPERATION = 3,
			END_OF_OPTION_LIST = 4,
		} OPTIONS;
		struct TCPOption {
			uint8_t kind; // Option kind (1 byte)
			uint8_t length; // Option length, including the kind and length fields (1 byte)
			uint16_t data; // Option data (variable length)

			TCPOption(uint8_t kind, uint16_t& data)
				: kind(kind), length(4), data(data) {}
		};

		typedef enum TCPFlags {
			URG = 0x01,
			ACK = 0x02,
			PSH = 0x03,
			RST = 0x04,
			SYN = 0x05,
			FIN = 0x06
		} FLAGS;

		typedef struct TCPHeader {
			SIZE_PORT _sourcePort;
			SIZE_PORT _destinationPort;
			SIZE_SEQ _SEQ;
			SIZE_ACK _ACK;
			unsigned _DO : 4;
			unsigned _RES : 3;
			SIZE_FLAGS _flags = 0; // Control Bits All flags are initially off
			SIZE_WINDOW _window;
			SIZE_CHECKSUM _checksum;
			SIZE_URGENT _urgent;
			// Use a map to store options.
			std::vector<TCPOption> _options;

			TCPHeader() {
				_flags |= TCPFlags::SYN;
			}
			TCPHeader(uint16_t sourcePort,
				uint16_t destinationPort,
				uint32_t SEQ,
				uint32_t ACK,
				unsigned DO,
				unsigned RES,
				uint16_t window,
				uint16_t checksum,
				uint16_t urgent,
				uint8_t flags = 0) : 
				_sourcePort(sourcePort), 
				_destinationPort(destinationPort), 
				_SEQ(SEQ), _ACK(ACK), 
				_DO(DO), _RES(RES), 
				_flags(flags), 
				_window(window), 
				_checksum(checksum),
				_urgent(urgent)
			{

			}
			~TCPHeader() {}

			// TCP header stuff put here later
		} HEADER;
		typedef struct TCPSegment {
			HEADER header;
			TCP_DATA_TYPE data;
			TCPSegment(uint16_t sourcePort,
				uint16_t destinationPort,
				uint32_t SEQ,
				uint32_t ACK,
				unsigned DO,
				unsigned reserved,
				uint16_t window,
				uint16_t checksum,
				uint16_t urgent,
				uint8_t flags = 0) : header(sourcePort,
					destinationPort,
					SEQ,
					ACK,
					DO,
					reserved,
					flags,
					window,
					checksum,
					urgent) {
				data.reserve(1460);
			}
			TCPSegment() = delete;
			TCPSegment(HEADER _header) : header(std::move(_header)) {
				data.reserve(1460);
			}
			~TCPSegment() {}
		} SEGMENT;
		typedef struct Timer {
			std::chrono::milliseconds duration;

			explicit Timer(int durationInMilliseconds) : duration(durationInMilliseconds) {}

			void wait() {
				std::this_thread::sleep_for(duration);
			}
		};
		
		class tsqueue {
		public:
			tsqueue() = default;
			tsqueue(const tsqueue&) = delete;

			virtual ~tsqueue() { clear(); }

			// Returns and maintains item at front of Queue
			const TCPSegment& front() {
				std::lock_guard<std::mutex> lock(muxQueue);
				return deqQueue.front();
			}

			// Returns and maintains item at back of Queue
			const TCPSegment& back() {
				std::lock_guard<std::mutex> lock(muxQueue);
				return deqQueue.back();
			}

			// Add an item to the back of Queue
			void push_back(const TCPSegment& item) {
				std::lock_guard<std::mutex> lock(muxQueue);
				deqQueue.emplace_back(std::move(item));

				// TODO: Too many locks - rework!
				std::unique_lock<std::mutex> u_lock(muxBlocking);
				cvBlocking.notify_one();
			}

			// Add an item to the front of Queue
			void push_front(const TCPSegment& item) {
				std::lock_guard<std::mutex> lock(muxQueue);
				deqQueue.emplace_front(std::move(item));

				// TODO: Too many locks - rework!
				std::unique_lock<std::mutex> u_lock(muxBlocking);
				cvBlocking.notify_one();
			}

			// Return true if Queue has no items
			bool empty() {
				std::lock_guard<std::mutex> lock(muxQueue);
				return deqQueue.empty();
			}

			// Return number of items in the Queue
			bool count() {
				std::lock_guard<std::mutex> lock(muxQueue);
				return deqQueue.size();
			}

			// Clears the Queue
			void clear() {
				std::lock_guard<std::mutex> lock(muxQueue);
				return deqQueue.clear();
			}

			// Removes and returns item from front of Queue
			// TODO: Fix the case for empty queue
			TCPSegment pop_front() {
				std::lock_guard<std::mutex> lock(muxQueue);
				auto t = std::move(deqQueue.front());
				deqQueue.pop_front();
				return t;
			}

			// Removes and returns item from back of Queue
			// TODO: Fix the case for empty queue
			TCPSegment pop_back() {
				std::lock_guard<std::mutex> lock(muxQueue);
				auto t = std::move(deqQueue.back());
				deqQueue.pop_back();
				return t;
			}

			void wait() {
				// TODO: Rework without loop
				while (empty()) {
					std::unique_lock<std::mutex> lock(muxBlocking);
					cvBlocking.wait(lock);
				}
			}
		// TODO: Too many Public things - rework the TCP to inherit rather than composite
		public:
			std::mutex muxQueue;
			std::deque<TCPSegment> deqQueue;
			std::condition_variable cvBlocking;
			std::mutex muxBlocking;
		};
	}
}


