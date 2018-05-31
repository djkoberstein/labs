#include <iostream>
#include <stdexcept>
#include <memory>

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"
#include "simple-web-server/client_http.hpp"
#include "simple-web-server/status_code.hpp"

using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;

enum class StatusEnum
{
    UNKOWN,
    CONNECTION,
    NOT_FOUND
};

template <class THttpClient = HttpClient>
class ClientUser
{
  public:
    ClientUser(THttpClient &client)
        : client_ (client) 
    {
        
    }

    StatusEnum GetStatus()
    {
        StatusEnum status = StatusEnum::UNKOWN;
        try
        {
            auto response = client_.request("GET", "/archive/cemetery/cemetery.htm");
            status = StatusEnum::CONNECTION;
            if (response->status_code == "404 Not Found")
            {
                status = StatusEnum::NOT_FOUND;
            }
        }
        catch (std::exception &e)
        {
            // no connection;
        }
        return status;
    }

  private:
    THttpClient &client_;
};

class Fake_Response
{
  public:
    std::string status_code;
};

class StatusCodeClient
{
  public:
    StatusCodeClient(std::string status_code)
        : status_code_(status_code)
    {
    }

    std::shared_ptr<Fake_Response> request(std::string, std::string)
    {
        std::shared_ptr<Fake_Response> response = std::make_shared<Fake_Response>();
        response->status_code = status_code_;
        return response;
    }

  private:
    std::string status_code_;
};

class ExceptionClient
{
  public:
    ExceptionClient()
    {
    }

    std::shared_ptr<Fake_Response> request(std::string, std::string)
    {
        throw std::runtime_error("");
        return std::make_shared<Fake_Response>();
    }
};

TEST_CASE("Test_Real_Client Exception")
{
    // Arrange
    HttpClient clientImpl("192.168.0.27:12480");
    clientImpl.config.timeout = 1;
    clientImpl.config.timeout_connect = 1;
    ClientUser<HttpClient> client(clientImpl);

    // Act
    StatusEnum status = client.GetStatus();

    // Assert
    REQUIRE(status == StatusEnum::UNKOWN);
}

TEST_CASE("Test_Real_Client 200 OK")
{
    // Arrange
    HttpClient clientImpl("terrykoberstein.com");
    ClientUser<HttpClient> client(clientImpl);

    // Act
    StatusEnum status = client.GetStatus();

    // Assert
    REQUIRE(status == StatusEnum::CONNECTION);
}

TEST_CASE("Test_Fake_Client 200 OK")
{
    // Arrange
    StatusCodeClient clientImpl("200 OK");
    ClientUser<StatusCodeClient> client(clientImpl);

    // Act
    StatusEnum status = client.GetStatus();

    // Assert
    REQUIRE(status == StatusEnum::CONNECTION);
}

TEST_CASE("Test_Fake_Client 404 Not Found")
{
    // Arrange
    StatusCodeClient clientImpl("404 Not Found");
    ClientUser<StatusCodeClient> client(clientImpl);

    // Act
    StatusEnum status = client.GetStatus();

    // Assert
    REQUIRE(status == StatusEnum::NOT_FOUND);
}

TEST_CASE("Test_Fake_Client Exception")
{
    // Arrange
    ExceptionClient clientImpl;
    ClientUser<ExceptionClient> client(clientImpl);

    // Act
    StatusEnum status = client.GetStatus();

    // Assert
    REQUIRE(status == StatusEnum::UNKOWN);
}
