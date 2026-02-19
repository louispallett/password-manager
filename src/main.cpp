#include "app/Application.h"

int main (void)
{
    app::Application app("vault.dat");
    app.run(app);

    return 0;
}
