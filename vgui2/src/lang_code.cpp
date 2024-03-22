#include "lang_code.h"
#include <string>
#include <map>

const static std::map<std::string, std::string> table = {
	{ "afrikaans", "af" },
	{ "albanian", "sq" },
	{ "arabic", "ar" },
	{ "basque", "eu" },
	{ "belarusian", "be" },
	{ "bulgarian", "bg" },
	{ "catalan", "ca" },
	{ "chinese_hk", "zh-hk" },
	{ "chinese_cn", "zh-cn" },
	{ "chinese_sg", "zh-sg" },
	{ "chinese_tw", "zh-tw" },
	{ "croatian", "hr" },
	{ "czech", "cs" },
	{ "danish", "da" },
	{ "dutch", "nl" },
	{ "english", "en" },
	{ "estonian", "et" },
	{ "faeroese", "fo" },
	{ "farsi", "fa" },
	{ "finnish", "fi" },
	{ "french", "fr" },
	{ "gaelic", "gd" },
	{ "german", "de" },
	{ "greek", "el" },
	{ "hebrew", "he" },
	{ "hindi", "hi" },
	{ "hungarian", "hu" },
	{ "icelandic", "is" },
	{ "indonesian", "id" },
	{ "irish", "ga" },
	{ "italian", "it" },
	{ "japanese", "ja" },
	{ "korean", "ko" },
	{ "kurdish", "ku" },
	{ "latvian", "lv" },
	{ "lithuanian", "lt" },
	{ "macedonian", "mk" },
	{ "malayalam", "ml" },
	{ "malaysian", "ms" },
	{ "maltese", "mt" },
	{ "norwegian", "no" },
	{ "polish", "pl" },
	{ "portuguese", "pt" },
	{ "punjabi", "pa" },
	{ "romanian", "ro" },
	{ "russian", "ru" },
	{ "serbian", "sr" },
	{ "slovak", "sk" },
	{ "slovenian", "sl" },
	{ "sorbian", "sb" },
	{ "spanish", "es" },
	{ "swedish", "sv" },
	{ "thai", "th" },
	{ "tsonga", "ts" },
	{ "tswana", "tn" },
	{ "turkish", "tr" },
	{ "ukrainian", "ua" },
	{ "urdu", "ur" },
	{ "venda", "ve" },
	{ "vietnamese", "vi" },
	{ "welsh", "cy" },
	{ "xhosa", "xh" },
	{ "yiddish", "ji" },
	{ "zulu", "zu" }
};

const char* ConvertLangNameToISO6391Code(const char* langName) {
	auto it = table.find(langName);
	return it != table.end() ? it->second.c_str() : "";
}