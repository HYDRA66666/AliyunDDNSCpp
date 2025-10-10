#include "command_handler.h"
#include "pch.h"

namespace HYDRA15::AliyunDDNSCpp
{
	void command_handler::getip(const std::list<std::string>&)
	{
        secretary::logger lgr = UNION_CREATE_LOGGER();
        lgr.debug(vslz.fetchip.data());
		commander::Command::excute(cmds.getipv4.cmd.data());
		commander::Command::excute(cmds.getipv6.cmd.data());
	}

    void command_handler::get_ipv4(const std::list<std::string>&)
    {
        secretary::logger lgr = UNION_CREATE_LOGGER();
        std::string ipurl = initializer::get_instance().ipv4url;

        lgr.debug(vslz.fetchip.data(), ipurl);
        httplib::SSLClient cli(ipurl);
        cli.set_connection_timeout(cfg.httpTimeout);
        auto res = cli.Get("/");
        if (res && res->status == 200)
        {
            lgr.info(vslz.gotipv4.data(), res->body);
            initializer::get_instance().ipv4 = res->body;
        }
        else if (res)
            throw std::runtime_error(std::format(
                vslz.httpFailure.data(),
                ipurl,
                res->status
            ));
        else
            throw std::runtime_error(std::format(
                vslz.httpFailureUnknown.data(),
                ipurl
            ));
    }

    void command_handler::get_ipv6(const std::list<std::string>&)
    {
        secretary::logger lgr = UNION_CREATE_LOGGER();
        std::string ipurl = initializer::get_instance().ipv6url;

        lgr.debug(vslz.fetchip.data(), ipurl);
        httplib::SSLClient cli(ipurl);
        cli.set_connection_timeout(cfg.httpTimeout);
        auto res = cli.Get("/");

        if (res && res->status == 200)
        {
            lgr.info(vslz.gotipv6.data(), res->body);
            initializer::get_instance().ipv6 = res->body;
        }
        else if (res)
            throw std::runtime_error(std::format(
                vslz.httpFailure.data(),
                ipurl,
                res->status
            ));
        else
            throw std::runtime_error(std::format(
                vslz.httpFailureUnknown.data(),
                ipurl
            ));
    }

}


