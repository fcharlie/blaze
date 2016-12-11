////
#include <cstdio>
#include <cstdlib>
#include "package.hpp"
#include <rapidjson/document.h>
#include <rapidjson/encodedstream.h>  // AutoUTFInputStream
#include <rapidjson/filereadstream.h> // FileReadStream
#include <rapidjson/pointer.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/reader.h>
using namespace rapidjson;
typedef GenericDocument<UTF16<>> WDocument;

bool Repositories::Initialize(const wchar_t *rfile) {
  WDocument doc;
  FILE *fp = nullptr;
  if (_wfopen_s(&fp, rfile, L"rb+") != 0) {
    return false;
  }
  char buffer[4096];
  FileReadStream bis(fp, buffer, sizeof(buffer));
  AutoUTFInputStream<unsigned, FileReadStream> eis(bis);
  if (doc.ParseStream<0, AutoUTF<unsigned>>(eis).HasParseError()) {
    return false;
  }

  return false;
}

bool RepositoriesInstalled::Discover(const wchar_t *rdfile) {
  WDocument doc;
  FILE *fp = nullptr;
  if (_wfopen_s(&fp, rdfile, L"rb+") != 0) {
    return false;
  }
  char buffer[4096];
  FileReadStream bis(fp, buffer, sizeof(buffer));
  AutoUTFInputStream<unsigned, FileReadStream> eis(bis);
  if (doc.ParseStream<0, AutoUTF<unsigned>>(eis).HasParseError()) {
    return false;
  }
  auto iter = doc.FindMember(L"Packages");
  if (iter == doc.MemberEnd()) {
    return true;
  }
  if (!iter->value.IsArray()) {
    return false;
  }
  /*
   {"name":"package name","enable":true,"version":"1.0.0"}
  */
  auto Ay = iter->value.GetArray();
  for (const auto &a : Ay) {
    if (!a.IsObject())
      continue;
    auto it = a.FindMember(L"name");
    if (it == a.MemberEnd() || !it->value.IsString())
      continue;
    LocalPackage lp;
	std::wstring name;
    name.assign(it->value.GetString(), it->value.GetStringLength());
    it = a.FindMember(L"version");
    if (it == a.MemberEnd() || !it->value.IsString())
      continue;
    lp.version.assign(it->value.GetString(), it->value.GetStringLength());
	it = a.FindMember(L"date");
	if (it != a.MemberEnd() && it->value.IsString()) {
		lp.installtime.assign(it->value.GetString(), it->value.GetStringLength());
	}
	it = a.FindMember(L"enable");
	if (it != a.MemberEnd() && it->value.IsBool()) {
		lp.enable = it->value.GetBool();
	} else {
		lp.enable = true;
	}
	packages.insert(std::pair<std::wstring, LocalPackage>(std::move(name), std::move(lp)));
  }
  for (auto &p : packages) {
	  wprintf(L"install %s\ttime: %s\tversion: %s\n", p.first.c_str(), p.second.installtime.c_str(), p.second.version.c_str());
  }
  return true;
}