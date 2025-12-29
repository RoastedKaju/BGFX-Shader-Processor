#ifndef SHADER_PROCESSOR_H_
#define SHADER_PROCESSOR_H_

#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace shader::processor::internal {
enum class ShaderFileType { Unknown, Vertex, Fragment };

/// Converts Enum `ShaderFileType` to WString
/// 
/// @param `ShaderFileType` enum
/// @return WString of shader type
inline std::wstring shaderTypeToWString(ShaderFileType type) {
  switch (type) {
    case shader::processor::internal::ShaderFileType::Unknown:
      return L"unknown";
    case shader::processor::internal::ShaderFileType::Vertex:
      return L"vertex";
    case shader::processor::internal::ShaderFileType::Fragment:
      return L"fragment";
    default:
      return L"unknown";
  }
}

inline bool runProcess(const std::wstring& exec_file,
                       const std::wstring& args) {
  STARTUPINFOW si{};
  si.cb = sizeof(si);

  PROCESS_INFORMATION pi{};

  // CreateProcessW requires a mutable buffer
  std::vector<wchar_t> cmd(args.begin(), args.end());
  cmd.push_back(L'\0');

  BOOL ok = CreateProcessW(exec_file.c_str(), cmd.data(), nullptr, nullptr,
                           FALSE, 0, nullptr, nullptr, &si, &pi);

  if (!ok) {
    DWORD err = GetLastError();
    std::wcerr << L"Failed to start process, error: " << err << std::endl;
    return false;
  }

  WaitForSingleObject(pi.hProcess, INFINITE);

  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);

  return true;
}

}  // namespace shader::processor::internal

namespace shader::processor {

/// Finds all regular files under the given directory path.
///
/// The search is not performed recursively. The returned list contains only
/// regular files; directories and other non-file entries are ignored.
///
/// @param directory Root directory to search.
/// @return A list of filesystem paths to all discovered files.
inline std::vector<std::filesystem::path> findShaderFiles(
    const std::filesystem::path& path) {

  namespace fs = std::filesystem;

  std::vector<fs::path> out_files;

  // check if given path is a folder and exists on disk
  if (!fs::exists(path) || !fs::is_directory(path)) {
    return out_files;
  }

  // iterate over files in folder
  for (const auto& directory_entry : fs::directory_iterator(path)) {
    if (fs::is_regular_file(directory_entry.path())) {
      // check extension
      if (directory_entry.path().extension() != ".sc") {
        continue;
      }
      // insert to out vector
      out_files.push_back(directory_entry.path());
    }
  }

  return out_files;
}

/// <summary>
///
/// </summary>
/// <param name="file"></param>
/// <returns></returns>
inline internal::ShaderFileType detectShaderFileType(
    const std::filesystem::path& file) {

  const auto stem = file.stem();

  if (stem.extension() == ".vs") {
    return internal::ShaderFileType::Vertex;
  } else if (stem.extension() == ".fs") {
    return internal::ShaderFileType::Fragment;
  }

  return internal::ShaderFileType::Unknown;
}

inline void processShaders(const std::vector<std::filesystem::path>& files,
                           const std::filesystem::path& shader_source_dir,
                           const std::filesystem::path& shader_bin_dir,
                           const std::filesystem::path& shader_tool_dir) {

  namespace fs = std::filesystem;
  namespace si = internal;

  // Setup paths
  const fs::path shader_exec_path =
      fs::absolute(shader_tool_dir / "shadercRelease.exe");
  const fs::path varying_file_path =
      fs::absolute(shader_tool_dir / "varying.def.sc");

  // create output shader folder if it does not exist
  if (!std::filesystem::exists(shader_bin_dir)) {
    std::filesystem::create_directory(shader_bin_dir);
  }

  for (const auto& file : files) {
    const std::string filename = file.filename().string();

    // Determine what type of shader file
    si::ShaderFileType shader_type = detectShaderFileType(file);

    // Skip if unknown file type
    if (shader_type == si::ShaderFileType::Unknown) {
      continue;
    }

    // setup input and output file paths
    fs::path input_file = fs::absolute(file);
    fs::path output_file = fs::absolute(shader_bin_dir / file.filename());
    // replace .sc with .bin
    output_file.replace_extension(".bin");

    std::cout << input_file.string() << std::endl;
    std::cout << output_file.string() << std::endl;

    std::wstring shader_type_wstr =
        shader_type == si::ShaderFileType::Vertex ? L"vertex" : L"fragment";

    std::wstringstream command;
    command << L"-f \"" << input_file.wstring() << L"\" " << L"-o \""
            << output_file.wstring() << L"\" " << L"--type " << shader_type_wstr
            << L" " << L"--platform windows " << L"--profile 120 " << L"-i \""
            << fs::absolute(shader_tool_dir).wstring() << L"\" "
            << L"--varyingdef \"" << varying_file_path.wstring() << L"\"";

    if (!si::runProcess(shader_exec_path.wstring(), command.str())) {
      std::cout << "Failed to process " << filename << std::endl;
    }
  }
}

}  // namespace shader::processor

#endif  // !SHADER_PROCESSOR_H_
