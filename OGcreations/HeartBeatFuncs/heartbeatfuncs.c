

#include <xdc/std.h>
#include "Board.h"


// Void heartBeatFxn(UArg limit, UArg LED_inc)
// {
//     /*A function which can be entered into by multiple threads, and light up unique LED's.
//     The maximal-allowed time interval and LED# are passed as arg's, then the blinking
//     rate oscillates between zero and 'limit'
//     */
//     int direction = 1; //Define whether the blinking interval grows or shrinks, at the beginning
//     int ms_time = 1 + (LED_inc*limit);
//     while (1) {
//         Task_sleep((unsigned int)ms_time);
//         GPIO_toggle(Board_LED0+LED_inc);
//         if(direction == 1){
//             ms_time++;
//             if(ms_time>limit){
//                 direction = -1;
//             }
//         }
//         if(direction == -1){
//             ms_time--;
//             if(ms_time<2){
//                 direction = 1;
//             }
//         }

//     }
// }