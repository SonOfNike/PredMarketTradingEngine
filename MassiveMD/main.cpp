#include "MDConnector.h"
#include "../../Utils/pin_thread_functions.h"

int main(int argc, char* argv[]) {
    MDConnector* con = MDConnector::getInstance();
    con->on_init();

    con->connect();
}