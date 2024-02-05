#include <Aurora.h>

namespace Aurora {

    class AuApp : public Application {
    public:

        AuApp(const ApplicationSpecs& specs) : Application(specs) {
            
        }

    };

    Application* CreateApplication() {
        ApplicationSpecs specs{};
        specs.Name = "AuroraApp";

        return new AuApp(specs);
    }

}
