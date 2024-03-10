#include <QCoreApplication>
#include <sentry.h>

int main(int argc, char** argv)
{
    sentry_options_t *options = sentry_options_new();
    sentry_options_set_dsn(options, "https://2a4119dd87be5bab7aca5133e58b9b2d@sentry.thevpncompany.net/33");
    // This is also the default-path. For further information and recommendations:
    // https://docs.sentry.io/platforms/native/configuration/options/#database-path
    sentry_options_set_database_path(options, ".sentry-native");
    sentry_options_set_release(options, "my-project-name@2.3.12");
    sentry_options_set_debug(options, 1);
    sentry_init(options);

    sentry_capture_event(sentry_value_new_message_event(
                             SENTRY_LEVEL_INFO,
                             "UBports test",
                            "It works!"
    ));

    return 1;
}
