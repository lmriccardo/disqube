#include "DisqubeLogger.hpp"

std::vector<std::string> Qube::Logging::DisqubeLogger::levels = 
        {"INFO", "WARNING", "ERROR", "JOB_POSTING"};

Qube::Logging::DisqubeLogger::DisqubeLogger(int id, bool logOnFile, const std::string &folder)
    : _Id(id), _logOnFile(logOnFile)
{
    // Create the path and check that it exists and it is a folder
    std::filesystem::path rootFolder(folder);
    _logFolder = rootFolder;

    // Format the string representing the optional filename
    std::stringstream ss; ss << "DisQube_Logging_" << id << ".log";
    std::string fileName = ss.str();
    _logFile = fileName;

    if (!logOnFile) return;

    // Check if the input path exists, if not creat it
    if (!std::filesystem::exists(rootFolder))
    {
        std::cout << "(*) [DisqubeLogger] Input Path does not eixsts ... creating it" << std::endl;

        if (std::filesystem::create_directory(rootFolder))
        {
            std::cout << "(*) [DisqubeLogger] Logging path successfully created: ";
            std::cout << std::filesystem::absolute(rootFolder).string() << std::endl;

            // Set specific permissions
            std::filesystem::permissions(rootFolder,
                std::filesystem::perms::owner_all  |
                std::filesystem::perms::group_read |
                std::filesystem::perms::others_read,
                std::filesystem::perm_options::replace
            );
        }
    }

    // If it exists and it is not a folder error
    if (!(std::filesystem::is_directory(rootFolder))) {
        std::cerr << "(!) [DisqubeLogger] Given input folder " << folder;
        std::cerr << " is not a folder" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Qube::Logging::DisqubeLogger::info(const std::string &msg)
{
    log(msg, LoggingType::INFO);
}

void Qube::Logging::DisqubeLogger::warning(const std::string &msg)
{
    log(msg, LoggingType::WARNING);
}

void Qube::Logging::DisqubeLogger::error(const std::string &msg)
{
    log(msg, LoggingType::ERROR);
}

void Qube::Logging::DisqubeLogger::jobPosting(const std::string &msg)
{
    log(msg, LoggingType::JOB_POSTING);
}

std::string Qube::Logging::DisqubeLogger::colorize(const std::string &word, LoggingType lvl, bool onFile)
{
    if (onFile) return word; // On file we should not use colors

    switch (lvl)
    {
        case LoggingType::INFO:
            return GREEN + word + NOCOLO;
        case LoggingType::WARNING:
            return YELLOW + word + NOCOLO;
        case LoggingType::ERROR:
            return RED + word + NOCOLO;
        case LoggingType::JOB_POSTING:
            return CYAN + word + NOCOLO;
    }

    return word;
}

void Qube::Logging::DisqubeLogger::log(const std::string &msg, LoggingType lvl)
{
    std::string level = DisqubeLogger::levels.at(lvl); // Take the level string

    // Create the filepath and use it if corresponding flag is true
    std::filesystem::path filePath(_logFile);
    std::filesystem::path logPath = _logFolder / filePath;

    // Take the timestamp and the date
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    
    // Generate the logging message
    std::stringstream ss;

    // When logging on file we do not need colorization
    if (!_logOnFile)
    {
        ss << "(" << colorize(level, lvl, _logOnFile) << ") <" << BLUE << "Disqube-" << _Id << NOCOLO << "> ";
        ss << MAGENTA << std::put_time(std::localtime(&currentTime), "[%Y-%m-%d %H:%M:%S]") << NOCOLO;
        ss << " " << msg;
    } else
    {
        ss << "(" << level << ") <Disqube-" << _Id << "> ";
        ss << std::put_time(std::localtime(&currentTime), "[%Y-%m-%d %H:%M:%S]");
        ss << " " << msg;
    }

    // Check the on-file flag and open the file if necessary
    if (_logOnFile)
    {
        std::ofstream outFile(logPath, std::ios::app);
        
        // Check if the file has been correctly opened
        if (outFile.is_open())
        {
            outFile << ss.str() << std::endl;
            outFile.close(); // Close the file after writing
            return;
        }

        std::cerr << "[DisqubeLogging::log] Unable to open the logging file ";
        std::cerr << logPath.string() << std::endl;
    }

    std::cout << ss.str() << std::endl;
}