#ifndef CALM_TRADER_CFG_H
#define CALM_TRADER_CFG_H
#include <string>


namespace calm {
    class Config { // a singleton
    public:
        // historical data config
        std::string const hist_default_download = "/home/wayne/calm/hist_download.yaml";
        int const hist_time_format = 2; // 1: 20231019 16:11:48 America/New_York 2: 1697746308
        int hist_window_sz = 2;

        // trading engine config
        char const* gateway_host = "";
        int gateway_port = 7497;
        int gateway_client_id = 0;

        // algo_engine
        std::string const algo_config_path = "/home/wayne/calm/bollinger_band.yaml";

        // member functions
        static Config const& get();

        Config(Config const&) = delete;
        void operator=(Config const&) = delete;
    private:
        Config();
    };

}
#endif //CALM_TRADER_CFG_H
