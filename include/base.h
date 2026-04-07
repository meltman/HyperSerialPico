/* base.h
*
*  MIT License
*
*  Copyright (c) 2023-2026 awawa-dev
*
*  https://github.com/awawa-dev/HyperSerialPico
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in all
*  copies or substantial portions of the Software.

*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*  SOFTWARE.
 */

#ifndef BASE_H
#define BASE_H

class Base
{
	// LED strip number
	int ledsNumber = 0;
	// NeoPixelBusLibrary primary object
	LED_DRIVER* ledStrip1 = nullptr;
	// NeoPixelBusLibrary second object
	LED_DRIVER2* ledStrip2 = nullptr;
	// frame is set and ready to render
	LED_DRIVER3* ledStrip3 = nullptr;
	// NeoPixelBusLibrary second object
	LED_DRIVER4* ledStrip4 = nullptr;
	// frame is set and ready to render
	bool readyToRender = false;

	public:
		// static data buffer for the loop
		volatile uint8_t buffer[MAX_BUFFER + 1] = {0};
		// handle to tasks
		TaskHandle_t processDataHandle = nullptr;
		TaskHandle_t processSerialHandle = nullptr;
		// semaphore to synchronize them
		semaphore_t serialSemaphore;
		semaphore_t receiverSemaphore;
		// current queue position
		volatile int queueCurrent = 0;
		// queue end position
		volatile int queueEnd = 0;

		inline int getLedsNumber()
		{
			return ledsNumber;
		}

		inline LED_DRIVER* getLedStrip1()
		{
			return ledStrip1;
		}

		inline LED_DRIVER2* getLedStrip2()
		{
			return ledStrip2;
		}

		inline LED_DRIVER3* getLedStrip3()
		{
			return ledStrip3;
		}

		inline LED_DRIVER4* getLedStrip4()
		{
			return ledStrip4;
		}

		void initLedStrip(int count)
		{
			if (ledStrip1 != nullptr)
			{
				delete ledStrip1;
				ledStrip1 = nullptr;
			}

			if (ledStrip2 != nullptr)
			{
				delete ledStrip2;
				ledStrip2 = nullptr;
			}

			if (ledStrip3 != nullptr)
			{
				delete ledStrip3;
				ledStrip3 = nullptr;
			}

			if (ledStrip4 != nullptr)
			{
				delete ledStrip4;
				ledStrip4 = nullptr;
			}

			ledsNumber = count;

			#if defined(FOURTH_SEGMENT_START_INDEX)
				if (ledsNumber > SECOND_SEGMENT_START_INDEX)
				{
					#if defined(NEOPIXEL_RGBW) || defined(NEOPIXEL_RGB)
						ledStrip1 = new LED_DRIVER(SECOND_SEGMENT_START_INDEX, DATA_PIN);
						ledStrip2 = new LED_DRIVER2(THIRD_SEGMENT_START_INDEX - SECOND_SEGMENT_START_INDEX, DATA_PIN);
						ledStrip3 = new LED_DRIVER3(FOURTH_SEGMENT_START_INDEX - THIRD_SEGMENT_START_INDEX, DATA_PIN);
						ledStrip4 = new LED_DRIVER4(ledsNumber - FOURTH_SEGMENT_START_INDEX, DATA_PIN);
					#else
						ledStrip1 = new LED_DRIVER(SECOND_SEGMENT_START_INDEX);
						ledStrip1->Begin(CLOCK_PIN, 12, DATA_PIN, 15);
						ledStrip2 = new LED_DRIVER2(THIRD_SEGMENT_START_INDEX - SECOND_SEGMENT_START_INDEX);
						ledStrip2->Begin(SECOND_SEGMENT_CLOCK_PIN, 12, SECOND_SEGMENT_DATA_PIN, 15);
						ledStrip3 = new LED_DRIVER3(FOURTH_SEGMENT_START_INDEX - THIRD_SEGMENT_START_INDEX);
						ledStrip3->Begin(THIRD_SEGMENT_CLOCK_PIN, 12, DATA_PIN, 15);
						ledStrip4 = new LED_DRIVER4(ledsNumber - FOURTH_SEGMENT_START_INDEX);
						ledStrip4->Begin(FOURTH_SEGMENT_CLOCK_PIN, 12, SECOND_SEGMENT_DATA_PIN, 15);
					#endif
				}
			#endif

			if (ledStrip1 == nullptr)
			{
				#if defined(NEOPIXEL_RGBW) || defined(NEOPIXEL_RGB)
					ledStrip1 = new LED_DRIVER(ledsNumber, DATA_PIN);
				#else
					ledStrip1 = new LED_DRIVER(ledsNumber, SPI_INTERFACE, DATA_PIN, CLOCK_PIN);
				#endif
			}
		}

		/**
		 * @brief Check if there is already prepared frame to display
		 *
		 * @return true
		 * @return false
		 */
		inline bool hasLateFrameToRender()
		{
			return readyToRender;
		}

		inline void dropLateFrame()
		{
			readyToRender = false;
		}

		inline void renderLeds(bool newFrame)
		{
			if (newFrame)
				readyToRender = true;

			if (readyToRender &&
				(ledStrip1 != nullptr && ledStrip1->isReadyBlocking()))
			{
				statistics.increaseShow();
				readyToRender = false;

				// display segments
				#if defined(SECOND_SEGMENT_START_INDEX)
					ledStrip1->renderAllLanes();
				#else
					ledStrip1->renderSingleLane();
				#endif
			}
		}

		inline bool setStripPixel(uint16_t pix, ColorDefinition &inputColor)
		{
		    if (pix < ledsNumber)
		    {
		        #if defined(FOURTH_SEGMENT_START_INDEX)
		            if (pix >= FOURTH_SEGMENT_START_INDEX) {
		                #if defined(FOURTH_SEGMENT_REVERSED)
		                    ledStrip4->SetPixel(ledsNumber - 1 - pix, inputColor);
		                #else
		                    ledStrip4->SetPixel(pix - FOURTH_SEGMENT_START_INDEX, inputColor);
		                #endif
		            }
		            else
		        #endif
		
		        #if defined(THIRD_SEGMENT_START_INDEX)
		            if (pix >= THIRD_SEGMENT_START_INDEX) {
		                #if defined(THIRD_SEGMENT_REVERSED)
		                    // Local index = (End of segment - 1) - current pixel
		                    ledStrip3->SetPixel(FOURTH_SEGMENT_START_INDEX - 1 - pix, inputColor);
		                #else
		                    ledStrip3->SetPixel(pix - THIRD_SEGMENT_START_INDEX, inputColor);
		                #endif
		            }
		            else
		        #endif
		
		        #if defined(SECOND_SEGMENT_START_INDEX)
		            if (pix >= SECOND_SEGMENT_START_INDEX) {
		                #if defined(SECOND_SEGMENT_REVERSED)
		                    ledStrip2->SetPixel(THIRD_SEGMENT_START_INDEX - 1 - pix, inputColor);
		                #else
		                    ledStrip2->SetPixel(pix - SECOND_SEGMENT_START_INDEX, inputColor);
		                #endif
		            }
		            else
		        #endif
		            {
		                // Default to first segment
		                ledStrip1->SetPixel(pix, inputColor);
		            }
		    }
		
		    return (pix + 1 < ledsNumber);
		}
		} base;

#endif
