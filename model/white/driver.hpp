#ifndef PVIVAX_MODEL_DRIVER
#define PVIVAX_MODEL_DRIVER

namespace scnXml {
    class Scenario;
}

namespace OM { namespace white {

void init_model( const scnXml::Scenario& scenario );

void run_model();

} }

#endif
