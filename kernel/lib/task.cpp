#include "lib/task.hpp"

extern "C" void switchToTask(kernel::task::ControlBlock* target);
kernel::task::ControlBlock* currentTask;

namespace kernel::task
{

void init()
{
    ControlBlock self;
    currentTask = &self;
    switchToTask(&self);
}

}
