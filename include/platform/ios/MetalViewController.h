#import <MetalKit/MetalKit.h>

#include <UIKit/UIKit.h>

#include <queue>

#include "platform/event.h"

@interface MetalViewController : UIViewController {

@public
std::queue<eng::Event> events_;

}

@end

