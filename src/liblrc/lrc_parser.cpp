//******************************************
//  Author : Yuwei Huang
//  Created On : Sat Feb 10 2018
//  File : lrc_parser.cc
//******************************************

#include "liblrc/lrc_parser.h"

#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace lrc {

namespace {

// G1: metadata key, G2: metadata value
static const std::wregex kMetadataExp(LR"exp(\[([a-zA-Z]+):([^\]]+)\])exp");

// G1: lyric
static const std::wregex kLyricExp(LR"exp(\]([^\]]*)\r?$)exp");

// G1: mm, G2: ss
static const std::wregex kTimestampExp(LR"exp(\[(\d\d:\d\d\.\d\d)\])exp");

void SetMetadata(Lyrics::Metadata* metadata,
                 const std::wstring& key,
                 const std::wstring& val) {
  if (key == L"ar")
    metadata->artist = val;
  else if (key == L"al")
    metadata->album = val;
  else if (key == L"ti")
    metadata->title = val;
  else if (key == L"au")
    metadata->lyricsWriter = val;
  else if (key == L"by")
    metadata->fileCreator = val;
  else if (key == L"offset") {
    metadata->adjustment = std::stoi(val);
  } else {
    // Unknown key. Do nothing.
  }
}

}  // namespace

LrcParser::LrcParser() {}
LrcParser::~LrcParser() {}

std::unique_ptr<Lyrics> LrcParser::ParseStream(
    std::wistream* stream) const {
  Lyrics::Metadata metadata {L"", L"", L"", L"", L"", 0};
  std::vector<Lyrics::LyricLine> lyric_lines;

  for (std::wstring line; std::getline(*stream, line); ) {
    std::wsmatch metadata_match;
    // Assume one metadata tag per line.
    if (std::regex_search(line, metadata_match, kMetadataExp)) {
      std::wstring key = metadata_match[1];
      std::wstring val = metadata_match[2];
      SetMetadata(&metadata, key, val);
      continue;
    }

    std::wsmatch lyric_match;
    if (!std::regex_search(line, lyric_match, kLyricExp)) {
      // No lyric. The line is malformed.
      continue;
    }
    std::wstring lyric = lyric_match[1];

    // Load timestamps and push lyric lines.
    std::wstring::const_iterator search_start(line.cbegin());
    std::wsmatch timestamp_match;
    while (regex_search(
        search_start, line.cend(), timestamp_match, kTimestampExp)) {
      int32_t time_ms = LrcParser::TimeStringToMilliseconds(timestamp_match[1]);
      Lyrics::LyricLine lyric_line;
      lyric_line.start_time = time_ms;
      lyric_line.lyric = lyric;
      lyric_lines.push_back(lyric_line);

      search_start += timestamp_match.position() + timestamp_match.length();
    }
  }
  return std::unique_ptr<Lyrics>(new Lyrics(metadata, std::move(lyric_lines)));
}

int32_t LrcParser::TimeStringToMilliseconds(const std::wstring& time_str) {
    static const std::wregex s_kTimeRegex(LR"exp(^(\d+):(\d\d(\.?\d+)?)$)exp");
    std::wsmatch match;
    if (!std::regex_search(time_str, match, s_kTimeRegex)) {
        return -1;
    }
    uint32_t minutes = std::stoi(match[1]);
    float seconds = std::stof(match[2]);
    if (seconds >= 60) {
        return -1;
    }
    return minutes * 60000 + static_cast<int32_t>(seconds * 1000);
}

std::unique_ptr<Lyrics> LrcParser::ParseString(
    const std::wstring& lrc_string) const {
  std::wstringstream stream(lrc_string);
  return ParseStream(&stream);
}

std::unique_ptr<Lyrics> LrcParser::ParseFile(
    const std::wstring& filename) const {
  std::wifstream stream(filename);
  if (!stream.is_open()) {
    return nullptr;
  }
  std::unique_ptr<Lyrics> lyrics = ParseStream(&stream);
  stream.close();
  return lyrics;
}
}  // namespace lrc
