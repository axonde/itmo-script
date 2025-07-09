#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <interpreter>
#include <ArgParser.h>

int main(int argc, char** argv) {

    const std::string non_existant_path = "/..not-set../";

    ArgumentParser::ArgParser parser("ITMO Script");
    parser.AddHelp('h', "help", "Interpreted Python/Lua like programming language.\nMade by @axonde with love.");
    parser.AddStringArgument('i', "input", "interpreter input stream [path]").Default(non_existant_path);
    parser.AddStringArgument('o', "output", "interpreter output stream [path]").Default(non_existant_path);
    parser.AddStringArgument('e', "error", "interpreter error stream [path]").Default(non_existant_path);
    parser.AddStringArgument('f', "file", "interpret from file").Default(non_existant_path).Positional();

    if(!parser.Parse(argc, argv)) {
        Errors::GeneralErrors::WrongLaunch();
        std::cout << std::endl;
        std::cout << parser.HelpDescription() << std::endl;
        return 1;
    }

    std::istream* input = &std::cin;
    std::ostream* output = &std::cout;
    std::ostream* error = &std::cerr;
    std::ifstream input_file;
    std::ofstream output_file;
    std::ofstream error_file;

    if (std::string i = parser.GetStringValue("input"); i != non_existant_path) {
        {
            std::filesystem::path path(i);
            input_file.open(path, std::ios::in);
            if (!input_file.is_open()) {
                Errors::GeneralErrors::FailedOpenFile(path);
                return 1;
            }
            input = &input_file;
        }
    }

    if (std::string o = parser.GetStringValue("output"); o != non_existant_path) {
        {
            std::filesystem::path path(o);
            output_file.open(path, std::ios::out);
            if (!output_file.is_open()) {
                Errors::GeneralErrors::FailedOpenFile(path);
                return 1;
            }
            output = &output_file;
        }
    }

    if (std::string e = parser.GetStringValue("error"); e != non_existant_path) {
        {
            std::filesystem::path path(e);
            error_file.open(path, std::ios::out);
            if (!error_file.is_open()) {
                Errors::GeneralErrors::FailedOpenFile(path);
                return 1;
            }
        }
        error = &error_file;
    }

    Interpreter interpreter(*input, *output, *error);

    std::string file_path = parser.GetStringValue("file");

    if (file_path == non_existant_path) {
        Patterns::Welcome();

        while (true) {
            if (!interpreter.Interpret(std::cin, true)) {
                return 1;
            }
        }
        return 0;
    }

    std::fstream file(std::filesystem::path(file_path), std::ios::in);

    if (!file.is_open()) {
        Errors::GeneralErrors::FailedOpenFile(std::move(file_path));
        return 1;
    }

    if (!interpreter.Interpret(file, false)) {
        return 1;
    }

    return 0;
}
