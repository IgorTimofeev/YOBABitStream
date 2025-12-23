#pragma once

#include <cmath>
#include <algorithm>

namespace YOBA {
	class BitStream {
		public:
			explicit BitStream(const uint8_t* buffer, size_t bitIndex = 0) :
				_buffer(buffer)
			{
				setBitIndex(bitIndex);
			}

			inline const uint8_t* getBuffer() const {
				return _buffer;
			}

			inline size_t getBitIndex() const {
				return _bitIndex;
			}

			void setBitIndex(size_t value) {
				_bitIndex = value;

				updateByteIndicesFromBitIndex();
			}

			inline size_t getByteIndex() const {
				return _byteIndex;
			}

			size_t getBytesRead() const {
				return _byteBitIndex == 0 ? _byteIndex : _byteIndex + 1;
			}

			bool readBool() {
				const auto result = (*(_buffer + _byteIndex) >> _byteBitIndex) & 0b1;
				nextBit();

				return result;
			}

			uint8_t readUint8(uint8_t bits = 8) {
				return readUnsigned<uint8_t>(bits);
			}

			uint16_t readUint16(uint8_t bits = 16) {
				return readUnsigned<uint16_t>(bits);
			}

			int16_t readInt16(uint8_t bits = 16) {
				return readSigned<int16_t>(bits);
			}

			int32_t readInt32(uint8_t bits = 32) {
				return readSigned<int32_t>(bits);
			}

			uint32_t readUint32(uint8_t bits = 32) {
				return readUnsigned<uint32_t>(bits);
			}

		private:
			const uint8_t* _buffer;

			size_t _bitIndex = 0;
			size_t _byteIndex = 0;
			uint8_t _byteBitIndex = 0;

			void updateByteIndicesFromBitIndex() {
				_byteIndex = _bitIndex / 8;
				_byteBitIndex = _bitIndex % 8;
			}

			void nextBit() {
				_bitIndex++;
				_byteBitIndex++;

				if (_byteBitIndex >= 8) {
					_byteIndex++;
					_byteBitIndex = 0;
				}
			}

			template<typename TNumber>
			TNumber readUnsigned(uint8_t bits) {
				TNumber result = 0;

				for (int i = 0; i < bits; ++i) {
					result |= (((*(_buffer + _byteIndex) >> _byteBitIndex) & 0b1) << i);
					nextBit();
				}

				return result;
			}

			template<typename TNumber>
			TNumber readSigned(uint8_t bits) {
				// Magnitude
				TNumber magnitude = 0;

				for (int i = 0; i < bits - 1; ++i) {
					magnitude |= (((*(_buffer + _byteIndex) >> _byteBitIndex) & 0b1) << i);
					nextBit();
				}

				// Sign
				const auto sign = ((*(_buffer + _byteIndex) >> _byteBitIndex) & 0b1) == 1;
				nextBit();

				return sign ? -magnitude : magnitude;
			}
	};
}