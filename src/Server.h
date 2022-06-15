#ifndef _MONGOOSE_SERVER_H
#define _MONGOOSE_SERVER_H

#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <functional>

struct mg_connection;
struct mg_mgr;

/**
 * Wrapper for the Mongoose server
 */
namespace Mongoose
{
class Controller;
class Request;
class Response;
class Server
{
public:

    using FilterFunc = std::function<bool(const std::shared_ptr<Request> &request, const std::shared_ptr<Response> &response)>;
    /**
     * @brief Constructs the Server
     * @param bindAddress something like ":80", "0.0.0.0:80",  etc...
     * @param documentRoot Path to serve files from.
     */
    Server(const char *bindAddress = ":8080", const char *documentRoot = "www");
    virtual ~Server();


    /**
     * @brief openSSL 启动openSLL
     *
     * @param cert 签名后的证书
     * @param key 私钥
     * @param ca ca证书可为空
     */
    void openSSL(const char *cert, const char *key, const char *ca = "");

    void setMongooseLog(FILE *file, int level = 1);
    /**
     * @brief isRunning
     * @return true if the server is running
     */
    bool isRunning() const;

    /**
     * @brief start the server if it is already not started
     * @return true if the server is started
     */
    bool start();

    /**
     * @brief poll the server for incoming connections/requests for duration
     * @param duration - the number of milliseconds to poll the server for
     */
    void poll(int duration);

    /**
     * @brief stops the server if it is already running
     */
    void stop();

    /**
     * @brief registerController - add another controller that provides custom http routes
     */
    void registerController(Controller *c);

    /**
     * @brief deregisterController - removes the controller from all processing
     * @param c
     */
    void deregisterController(Controller *c);

    /**
     * @brief printStats prints basic statistics about the server to stdout
     */
    void printStats();

    /**
     * @brief handles    if server can deal method+url
     * @param method
     * @param url
     * @return true if the seerver handles a URL
     */
    bool handles(const std::string& method, const std::string& url);

    bool allowMultipleClients() const;
    void setAllowMultipleClients(bool value);

    size_t uploadSizeLimit() const;
    void setUploadSizeLimit(size_t limit);

    std::string bindAddress() const;
    void setBindAddress(const std::string& address);

    bool directoryListingEnabled() const;
    void setDirectoryListingEnabled(bool value);

    std::string documentRoot() const;
    void setDocumentRoot(const std::string& root);

    std::string indexFiles() const;
    void setIndexFiles(const std::string& files);

    std::string authDomain() const;
    void setAuthDomain(const std::string& domain);

    std::string basicAuthUsername() const;
    void setBasicAuthUsername(const std::string& user);

    std::string basicAuthPassword() const;
    void setBasicAuthPassword(const std::string& password);

    bool requiresBasicAuthentication() const;

    std::string ipAccessControlList() const;
    void setIpAccessControlList(const std::string& acl);

    std::string hiddenFilePattern() const;
    void setHiddenFilePattern(const std::string& pattern);

    std::string extraHeaders() const;
    void setExtraHeaders(const std::string& headers);

    std::string tmpDir() const;
    void setTmpDir(const std::string& tmpDir);

    void registFilter(FilterFunc func);

private:
    static void ev_handler(struct mg_connection *c, int ev, void *p, void* ud);

    bool handleRequest(const std::shared_ptr<Request>& request, const std::shared_ptr<Response>& response);

    bool mIsRunning;
    bool mOpenSSL;
    struct mg_mgr *mManager{nullptr};
    struct mg_connection *mConnection{nullptr};

    //ssl
    std::string mCertFile;
    std::string mKeyFile;
    std::string mCaCertFile;

    FilterFunc mFilterFunc;
    //Internals
    std::map<struct mg_connection*, std::shared_ptr<Request>> mCurrentRequests;
    std::map<struct mg_connection*, std::shared_ptr<Response>> mCurrentResponses;
    std::vector<Controller *> mControllers;

    // Bind options
    std::string mBindAddress;
    bool mAllowMultipleClients;

    // Http Options
    std::string mDocumentRoot;
    std::string mIndexFiles;
    std::string mEnableDirectoryListing;
    std::string mAuthDomain;
    std::string mBasicAuthUsername;
    std::string mBasicAuthPassword;
    std::string mIpAccessControlList;
    std::string mHiddenFilePattern;
    std::string mExtraHeaders;
    size_t mUploadSizeLimit;

    // Statistics
    int mRequests{0};
    int mStartTime{0};

    std::string mTmpDir;
};
}

#endif
