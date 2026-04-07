/* base.h */

#ifndef BASE_H
#define BASE_H

class Base
{
    int ledsNumber = 0;
    LED_DRIVER* ledStrip1 = nullptr;
    LED_DRIVER2* ledStrip2 = nullptr;
    LED_DRIVER3* ledStrip3 = nullptr;
    LED_DRIVER4* ledStrip4 = nullptr;
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
            if (ledStrip1) { delete ledStrip1; ledStrip1 = nullptr; }
            if (ledStrip2) { delete ledStrip2; ledStrip2 = nullptr; }
            if (ledStrip3) { delete ledStrip3; ledStrip3 = nullptr; }
            if (ledStrip4) { delete ledStrip4; ledStrip4 = nullptr; }

            ledsNumber = count;

            #if defined(FOURTH_SEGMENT_START_INDEX) && defined(THIRD_SEGMENT_START_INDEX) && defined(SECOND_SEGMENT_START_INDEX)
                if (ledsNumber > FOURTH_SEGMENT_START_INDEX)
                {
                    #if defined(NEOPIXEL_RGBW) || defined(NEOPIXEL_RGB)
                        ledStrip1 = new LED_DRIVER(SECOND_SEGMENT_START_INDEX, DATA_PIN);
                        ledStrip2 = new LED_DRIVER2(THIRD_SEGMENT_START_INDEX - SECOND_SEGMENT_START_INDEX, SECOND_SEGMENT_DATA_PIN);
                        ledStrip3 = new LED_DRIVER3(FOURTH_SEGMENT_START_INDEX - THIRD_SEGMENT_START_INDEX, THIRD_SEGMENT_DATA_PIN);
                        ledStrip4 = new LED_DRIVER4(ledsNumber - FOURTH_SEGMENT_START_INDEX, FOURTH_SEGMENT_DATA_PIN);
                    #else
                        ledStrip1 = new LED_DRIVER(SECOND_SEGMENT_START_INDEX);
                        ledStrip1->Begin(CLOCK_PIN, 12, DATA_PIN, 15);
                        
                        ledStrip2 = new LED_DRIVER2(THIRD_SEGMENT_START_INDEX - SECOND_SEGMENT_START_INDEX);
                        ledStrip2->Begin(SECOND_SEGMENT_CLOCK_PIN, 12, SECOND_SEGMENT_DATA_PIN, 16);
                        
                        ledStrip3 = new LED_DRIVER3(FOURTH_SEGMENT_START_INDEX - THIRD_SEGMENT_START_INDEX);
                        ledStrip3->Begin(THIRD_SEGMENT_CLOCK_PIN, 12, THIRD_SEGMENT_DATA_PIN, 17);
                        
                        ledStrip4 = new LED_DRIVER4(ledsNumber - FOURTH_SEGMENT_START_INDEX);
                        ledStrip4->Begin(FOURTH_SEGMENT_CLOCK_PIN, 12, FOURTH_SEGMENT_DATA_PIN, 18);
                    #endif
                }
            #endif

            // Fallback for single strip if indexing isn't defined or ledsNumber is too small
            if (ledStrip1 == nullptr)
            {
                #if defined(NEOPIXEL_RGBW) || defined(NEOPIXEL_RGB)
                    ledStrip1 = new LED_DRIVER(ledsNumber, DATA_PIN);
                #else
                    ledStrip1 = new LED_DRIVER(ledsNumber, SPI_INTERFACE, DATA_PIN, CLOCK_PIN);
                #endif
            }
        }

        // ... (renderLeds and setStripPixel functions from previous response)
} base;

#endif
