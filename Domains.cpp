#include "gb-include.h"

#include "HashTableX.h"
#include "Domains.h"
#include "Mem.h"

static bool isTLD ( char *tld, int32_t tldLen );

char *getDomainOfIp ( char *host , int32_t hostLen , int32_t *dlen ) {
	// get host length
	//int32_t hostLen = gbstrlen(host);
	// if ip != 0 then host is a numeric ip, point to first 3 #'s
	char *s = host + hostLen - 1;
	while ( s > host && *s!='.' ) s--;
	// if no '.' return NULL and 0
	if ( s == host ) { *dlen = 0; return NULL; }
	// otherwise, set length
	*dlen = s - host;
	// return the first 3 #'s (1.2.3) as the domain
	return host;
}


char *getDomain ( char *host , int32_t hostLen , char *tld , int32_t *dlen ) {
	// assume no domain 
	*dlen = 0;
	// get host length
	//int32_t hostLen = gbstrlen(host);
	// get the tld in host, if any, if not, it returns NULL
	char *s = tld; // getTLD ( host , hostLen );
	// return NULL if host contains no valid tld
	if ( ! s ) return NULL;
	// if s is host we just have tld
	if ( s == host ) return NULL;
	// there MUST be a period before s
	s--; if ( *s != '.' ) return NULL;
	// back up over the period
	s--;
	// now go back until s hits "host" or another period
	while ( s > host && *s !='.' ) s--;
	// . now *s=='.' or s==host
	// . if s is host then "host" is an acceptable domain w/o a hostname
	// . fix http://.xyz.com/...... by checking for period
	if ( s == host && *s !='.' ) { *dlen = hostLen; return s; }
	// skip s forward over the period to point to domain name
	s++;
	// set domain length
	*dlen = hostLen - ( s - host );
	return s;
}

// host must be NULL terminated
char *getTLD ( char *host , int32_t hostLen ) {
	if(hostLen==0)
		return NULL;
	// make "s" point to last period in the host
	//char *s = host + gbstrlen(host) - 1;
	char *hostEnd = host + hostLen;
	char *s       = hostEnd - 1;
	while ( s > host && *s !='.' ) s--;
	// point to the tld in question
	char *t  = s;
	if ( *t == '.' ) t++; 
	// reset our current tld ptr
	char *tld = NULL;
	// is t a valid tld? if so, set "tld" to "t".
	if ( isTLD ( t , hostEnd - t ) ) tld = t;
	// host had no period at most we had just a tld so return NULL
	if ( s == host ) return tld;

	// back up over last period
	s--;
	// just because it's in table doesn't mean we can't try going up more
	while ( s > host && *s !='.' ) s--;
	// point to the tld in question
	t  = s;
	if ( *t == '.' ) t++; 
	// is t a valid tld? if so, set "tld" to "t".
	if ( isTLD ( t , hostEnd - t ) ) tld = t;
	// host had no period at most we had just a tld so return NULL
	if ( s == host ) return tld;


	// . now only 1 tld has 2 period and that is "LKD.CO.IM"
	// . so waste another iteration for that (TODO: speed up?)
	// . back up over last period
	s--;
	// just because it's in table doesn't mean we can't try going up more
	while ( s > host && *s !='.' ) s--;
	// point to the tld in question
	t  = s;
	if ( *t == '.' ) t++; 
	// is t a valid tld? if so, set "tld" to "t".
	if ( isTLD ( t , hostEnd - t ) ) tld = t;
	// we must have gotten the tld by this point, if there was a valid one
	return tld;
}

//static TermTable  s_table(false);
static HashTableX s_table;
static bool isTLD ( char *tld , int32_t tldLen ) {

	int32_t pcount = 0;
	// now they are random!
	for ( int32_t i = 0 ; i < tldLen ; i++ ) {
		// period count
		if ( tld[i] == '.' ) { pcount++; continue; }
		if ( ! is_alnum_a(tld[i]) && tld[i] != '-' ) return false;
	}

	if ( pcount == 0 ) return true;
	if ( pcount >= 2 ) return false;

	// otherwise, if one period, check table to see if qualified

	// we use this as our hashtable
	static bool       s_isInitialized = false;
	// . i shrunk this list a lot
	// . see backups for the hold list
	static const char * const s_tlds[] = {

	  // From: https://data.iana.org/TLD/tlds-alpha-by-domain.txt
	"AAA",
	"AARP",
	"ABB",
	"ABBOTT",
	"ABBVIE",
	"ABOGADO",
	"ABUDHABI",
	"AC",
	"ACADEMY",
	"ACCENTURE",
	"ACCOUNTANT",
	"ACCOUNTANTS",
	"ACO",
	"ACTIVE",
	"ACTOR",
	"AD",
	"ADAC",
	"ADS",
	"ADULT",
	"AE",
	"AEG",
	"AERO",
	"AF",
	"AFL",
	"AG",
	"AGAKHAN",
	"AGENCY",
	"AI",
	"AIG",
	"AIRFORCE",
	"AIRTEL",
	"AKDN",
	"AL",
	"ALIBABA",
	"ALIPAY",
	"ALLFINANZ",
	"ALLY",
	"ALSACE",
	"AM",
	"AMICA",
	"AMSTERDAM",
	"ANALYTICS",
	"ANDROID",
	"ANQUAN",
	"AO",
	"APARTMENTS",
	"APP",
	"APPLE",
	"AQ",
	"AQUARELLE",
	"AR",
	"ARAMCO",
	"ARCHI",
	"ARMY",
	"ARPA",
	"ARTE",
	"AS",
	"ASIA",
	"ASSOCIATES",
	"AT",
	"ATTORNEY",
	"AU",
	"AUCTION",
	"AUDI",
	"AUDIO",
	"AUTHOR",
	"AUTO",
	"AUTOS",
	"AVIANCA",
	"AW",
	"AWS",
	"AX",
	"AXA",
	"AZ",
	"AZURE",
	"BA",
	"BABY",
	"BAIDU",
	"BAND",
	"BANK",
	"BAR",
	"BARCELONA",
	"BARCLAYCARD",
	"BARCLAYS",
	"BAREFOOT",
	"BARGAINS",
	"BAUHAUS",
	"BAYERN",
	"BB",
	"BBC",
	"BBVA",
	"BCG",
	"BCN",
	"BD",
	"BE",
	"BEATS",
	"BEER",
	"BENTLEY",
	"BERLIN",
	"BEST",
	"BET",
	"BF",
	"BG",
	"BH",
	"BHARTI",
	"BI",
	"BIBLE",
	"BID",
	"BIKE",
	"BING",
	"BINGO",
	"BIO",
	"BIZ",
	"BJ",
	"BLACK",
	"BLACKFRIDAY",
	"BLOOMBERG",
	"BLUE",
	"BM",
	"BMS",
	"BMW",
	"BN",
	"BNL",
	"BNPPARIBAS",
	"BO",
	"BOATS",
	"BOEHRINGER",
	"BOM",
	"BOND",
	"BOO",
	"BOOK",
	"BOOTS",
	"BOSCH",
	"BOSTIK",
	"BOT",
	"BOUTIQUE",
	"BR",
	"BRADESCO",
	"BRIDGESTONE",
	"BROADWAY",
	"BROKER",
	"BROTHER",
	"BRUSSELS",
	"BS",
	"BT",
	"BUDAPEST",
	"BUGATTI",
	"BUILD",
	"BUILDERS",
	"BUSINESS",
	"BUY",
	"BUZZ",
	"BV",
	"BW",
	"BY",
	"BZ",
	"BZH",
	"CA",
	"CAB",
	"CAFE",
	"CAL",
	"CALL",
	"CAMERA",
	"CAMP",
	"CANCERRESEARCH",
	"CANON",
	"CAPETOWN",
	"CAPITAL",
	"CAR",
	"CARAVAN",
	"CARDS",
	"CARE",
	"CAREER",
	"CAREERS",
	"CARS",
	"CARTIER",
	"CASA",
	"CASH",
	"CASINO",
	"CAT",
	"CATERING",
	"CBA",
	"CBN",
	"CC",
	"CD",
	"CEB",
	"CENTER",
	"CEO",
	"CERN",
	"CF",
	"CFA",
	"CFD",
	"CG",
	"CH",
	"CHANEL",
	"CHANNEL",
	"CHASE",
	"CHAT",
	"CHEAP",
	"CHLOE",
	"CHRISTMAS",
	"CHROME",
	"CHURCH",
	"CI",
	"CIPRIANI",
	"CIRCLE",
	"CISCO",
	"CITIC",
	"CITY",
	"CITYEATS",
	"CK",
	"CL",
	"CLAIMS",
	"CLEANING",
	"CLICK",
	"CLINIC",
	"CLINIQUE",
	"CLOTHING",
	"CLOUD",
	"CLUB",
	"CLUBMED",
	"CM",
	"CN",
	"CO",
	"COACH",
	"CODES",
	"COFFEE",
	"COLLEGE",
	"COLOGNE",
	"COM",
	"COMMBANK",
	"COMMUNITY",
	"COMPANY",
	"COMPARE",
	"COMPUTER",
	"COMSEC",
	"CONDOS",
	"CONSTRUCTION",
	"CONSULTING",
	"CONTACT",
	"CONTRACTORS",
	"COOKING",
	"COOL",
	"COOP",
	"CORSICA",
	"COUNTRY",
	"COUPON",
	"COUPONS",
	"COURSES",
	"CR",
	"CREDIT",
	"CREDITCARD",
	"CREDITUNION",
	"CRICKET",
	"CROWN",
	"CRS",
	"CRUISES",
	"CSC",
	"CU",
	"CUISINELLA",
	"CV",
	"CW",
	"CX",
	"CY",
	"CYMRU",
	"CYOU",
	"CZ",
	"DABUR",
	"DAD",
	"DANCE",
	"DATE",
	"DATING",
	"DATSUN",
	"DAY",
	"DCLK",
	"DE",
	"DEALER",
	"DEALS",
	"DEGREE",
	"DELIVERY",
	"DELL",
	"DELOITTE",
	"DELTA",
	"DEMOCRAT",
	"DENTAL",
	"DENTIST",
	"DESI",
	"DESIGN",
	"DEV",
	"DIAMONDS",
	"DIET",
	"DIGITAL",
	"DIRECT",
	"DIRECTORY",
	"DISCOUNT",
	"DJ",
	"DK",
	"DM",
	"DNP",
	"DO",
	"DOCS",
	"DOG",
	"DOHA",
	"DOMAINS",
	"DOWNLOAD",
	"DRIVE",
	"DUBAI",
	"DURBAN",
	"DVAG",
	"DZ",
	"EARTH",
	"EAT",
	"EC",
	"EDEKA",
	"EDU",
	"EDUCATION",
	"EE",
	"EG",
	"EMAIL",
	"EMERCK",
	"ENERGY",
	"ENGINEER",
	"ENGINEERING",
	"ENTERPRISES",
	"EPSON",
	"EQUIPMENT",
	"ER",
	"ERNI",
	"ES",
	"ESQ",
	"ESTATE",
	"ET",
	"EU",
	"EUROVISION",
	"EUS",
	"EVENTS",
	"EVERBANK",
	"EXCHANGE",
	"EXPERT",
	"EXPOSED",
	"EXPRESS",
	"EXTRASPACE",
	"FAGE",
	"FAIL",
	"FAIRWINDS",
	"FAITH",
	"FAMILY",
	"FAN",
	"FANS",
	"FARM",
	"FASHION",
	"FAST",
	"FEEDBACK",
	"FERRERO",
	"FI",
	"FILM",
	"FINAL",
	"FINANCE",
	"FINANCIAL",
	"FIRESTONE",
	"FIRMDALE",
	"FISH",
	"FISHING",
	"FIT",
	"FITNESS",
	"FJ",
	"FK",
	"FLICKR",
	"FLIGHTS",
	"FLORIST",
	"FLOWERS",
	"FLSMIDTH",
	"FLY",
	"FM",
	"FO",
	"FOO",
	"FOOTBALL",
	"FORD",
	"FOREX",
	"FORSALE",
	"FORUM",
	"FOUNDATION",
	"FOX",
	"FR",
	"FRESENIUS",
	"FRL",
	"FROGANS",
	"FRONTIER",
	"FTR",
	"FUND",
	"FURNITURE",
	"FUTBOL",
	"FYI",
	"GA",
	"GAL",
	"GALLERY",
	"GALLO",
	"GALLUP",
	"GAME",
	"GARDEN",
	"GB",
	"GBIZ",
	"GD",
	"GDN",
	"GE",
	"GEA",
	"GENT",
	"GENTING",
	"GF",
	"GG",
	"GGEE",
	"GH",
	"GI",
	"GIFT",
	"GIFTS",
	"GIVES",
	"GIVING",
	"GL",
	"GLASS",
	"GLE",
	"GLOBAL",
	"GLOBO",
	"GM",
	"GMAIL",
	"GMBH",
	"GMO",
	"GMX",
	"GN",
	"GOLD",
	"GOLDPOINT",
	"GOLF",
	"GOO",
	"GOOG",
	"GOOGLE",
	"GOP",
	"GOT",
	"GOV",
	"GP",
	"GQ",
	"GR",
	"GRAINGER",
	"GRAPHICS",
	"GRATIS",
	"GREEN",
	"GRIPE",
	"GROUP",
	"GS",
	"GT",
	"GU",
	"GUCCI",
	"GUGE",
	"GUIDE",
	"GUITARS",
	"GURU",
	"GW",
	"GY",
	"HAMBURG",
	"HANGOUT",
	"HAUS",
	"HDFCBANK",
	"HEALTH",
	"HEALTHCARE",
	"HELP",
	"HELSINKI",
	"HERE",
	"HERMES",
	"HIPHOP",
	"HITACHI",
	"HIV",
	"HK",
	"HM",
	"HN",
	"HOCKEY",
	"HOLDINGS",
	"HOLIDAY",
	"HOMEDEPOT",
	"HOMES",
	"HONDA",
	"HORSE",
	"HOST",
	"HOSTING",
	"HOTELES",
	"HOTMAIL",
	"HOUSE",
	"HOW",
	"HR",
	"HSBC",
	"HT",
	"HTC",
	"HU",
	"HYUNDAI",
	"IBM",
	"ICBC",
	"ICE",
	"ICU",
	"ID",
	"IE",
	"IFM",
	"IINET",
	"IL",
	"IM",
	"IMAMAT",
	"IMMO",
	"IMMOBILIEN",
	"IN",
	"INDUSTRIES",
	"INFINITI",
	"INFO",
	"ING",
	"INK",
	"INSTITUTE",
	"INSURANCE",
	"INSURE",
	"INT",
	"INTERNATIONAL",
	"INVESTMENTS",
	"IO",
	"IPIRANGA",
	"IQ",
	"IR",
	"IRISH",
	"IS",
	"ISELECT",
	"ISMAILI",
	"IST",
	"ISTANBUL",
	"IT",
	"ITAU",
	"IWC",
	"JAGUAR",
	"JAVA",
	"JCB",
	"JCP",
	"JE",
	"JETZT",
	"JEWELRY",
	"JLC",
	"JLL",
	"JM",
	"JMP",
	"JNJ",
	"JO",
	"JOBS",
	"JOBURG",
	"JOT",
	"JOY",
	"JP",
	"JPMORGAN",
	"JPRS",
	"JUEGOS",
	"KAUFEN",
	"KDDI",
	"KE",
	"KERRYHOTELS",
	"KERRYLOGISTICS",
	"KERRYPROPERTIES",
	"KFH",
	"KG",
	"KH",
	"KI",
	"KIA",
	"KIM",
	"KINDER",
	"KITCHEN",
	"KIWI",
	"KM",
	"KN",
	"KOELN",
	"KOMATSU",
	"KP",
	"KPMG",
	"KPN",
	"KR",
	"KRD",
	"KRED",
	"KUOKGROUP",
	"KW",
	"KY",
	"KYOTO",
	"KZ",
	"LA",
	"LACAIXA",
	"LAMBORGHINI",
	"LAMER",
	"LANCASTER",
	"LAND",
	"LANDROVER",
	"LANXESS",
	"LASALLE",
	"LAT",
	"LATROBE",
	"LAW",
	"LAWYER",
	"LB",
	"LC",
	"LDS",
	"LEASE",
	"LECLERC",
	"LEGAL",
	"LEXUS",
	"LGBT",
	"LI",
	"LIAISON",
	"LIDL",
	"LIFE",
	"LIFEINSURANCE",
	"LIFESTYLE",
	"LIGHTING",
	"LIKE",
	"LIMITED",
	"LIMO",
	"LINCOLN",
	"LINDE",
	"LINK",
	"LIPSY",
	"LIVE",
	"LIVING",
	"LIXIL",
	"LK",
	"LOAN",
	"LOANS",
	"LOCUS",
	"LOL",
	"LONDON",
	"LOTTE",
	"LOTTO",
	"LOVE",
	"LR",
	"LS",
	"LT",
	"LTD",
	"LTDA",
	"LU",
	"LUPIN",
	"LUXE",
	"LUXURY",
	"LV",
	"LY",
	"MA",
	"MADRID",
	"MAIF",
	"MAISON",
	"MAKEUP",
	"MAN",
	"MANAGEMENT",
	"MANGO",
	"MARKET",
	"MARKETING",
	"MARKETS",
	"MARRIOTT",
	"MBA",
	"MC",
	"MD",
	"ME",
	"MED",
	"MEDIA",
	"MEET",
	"MELBOURNE",
	"MEME",
	"MEMORIAL",
	"MEN",
	"MENU",
	"MEO",
	"MG",
	"MH",
	"MIAMI",
	"MICROSOFT",
	"MIL",
	"MINI",
	"MK",
	"ML",
	"MLS",
	"MM",
	"MMA",
	"MN",
	"MO",
	"MOBI",
	"MOBILY",
	"MODA",
	"MOE",
	"MOI",
	"MOM",
	"MONASH",
	"MONEY",
	"MONTBLANC",
	"MORMON",
	"MORTGAGE",
	"MOSCOW",
	"MOTORCYCLES",
	"MOV",
	"MOVIE",
	"MOVISTAR",
	"MP",
	"MQ",
	"MR",
	"MS",
	"MT",
	"MTN",
	"MTPC",
	"MTR",
	"MU",
	"MUSEUM",
	"MUTUAL",
	"MUTUELLE",
	"MV",
	"MW",
	"MX",
	"MY",
	"MZ",
	"NA",
	"NADEX",
	"NAGOYA",
	"NAME",
	"NATURA",
	"NAVY",
	"NC",
	"NE",
	"NEC",
	"NET",
	"NETBANK",
	"NETWORK",
	"NEUSTAR",
	"NEW",
	"NEWS",
	"NEXT",
	"NEXTDIRECT",
	"NEXUS",
	"NF",
	"NG",
	"NGO",
	"NHK",
	"NI",
	"NICO",
	"NIKON",
	"NINJA",
	"NISSAN",
	"NISSAY",
	"NL",
	"NO",
	"NOKIA",
	"NORTHWESTERNMUTUAL",
	"NORTON",
	"NOWRUZ",
	"NP",
	"NR",
	"NRA",
	"NRW",
	"NTT",
	"NU",
	"NYC",
	"NZ",
	"OBI",
	"OFFICE",
	"OKINAWA",
	"OLAYAN",
	"OM",
	"OMEGA",
	"ONE",
	"ONG",
	"ONL",
	"ONLINE",
	"OOO",
	"ORACLE",
	"ORANGE",
	"ORG",
	"ORGANIC",
	"ORIGINS",
	"OSAKA",
	"OTSUKA",
	"OVH",
	"PA",
	"PAGE",
	"PAMPEREDCHEF",
	"PANERAI",
	"PARIS",
	"PARS",
	"PARTNERS",
	"PARTS",
	"PARTY",
	"PASSAGENS",
	"PE",
	"PET",
	"PF",
	"PG",
	"PH",
	"PHARMACY",
	"PHILIPS",
	"PHOTO",
	"PHOTOGRAPHY",
	"PHOTOS",
	"PHYSIO",
	"PIAGET",
	"PICS",
	"PICTET",
	"PICTURES",
	"PID",
	"PIN",
	"PING",
	"PINK",
	"PIZZA",
	"PK",
	"PL",
	"PLACE",
	"PLAY",
	"PLAYSTATION",
	"PLUMBING",
	"PLUS",
	"PM",
	"PN",
	"POHL",
	"POKER",
	"PORN",
	"POST",
	"PR",
	"PRAXI",
	"PRESS",
	"PRO",
	"PROD",
	"PRODUCTIONS",
	"PROF",
	"PROGRESSIVE",
	"PROMO",
	"PROPERTIES",
	"PROPERTY",
	"PROTECTION",
	"PS",
	"PT",
	"PUB",
	"PW",
	"PWC",
	"PY",
	"QA",
	"QPON",
	"QUEBEC",
	"QUEST",
	"RACING",
	"RE",
	"READ",
	"REALTOR",
	"REALTY",
	"RECIPES",
	"RED",
	"REDSTONE",
	"REDUMBRELLA",
	"REHAB",
	"REISE",
	"REISEN",
	"REIT",
	"REN",
	"RENT",
	"RENTALS",
	"REPAIR",
	"REPORT",
	"REPUBLICAN",
	"REST",
	"RESTAURANT",
	"REVIEW",
	"REVIEWS",
	"REXROTH",
	"RICH",
	"RICOH",
	"RIO",
	"RIP",
	"RO",
	"ROCHER",
	"ROCKS",
	"RODEO",
	"ROOM",
	"RS",
	"RSVP",
	"RU",
	"RUHR",
	"RUN",
	"RW",
	"RWE",
	"RYUKYU",
	"SA",
	"SAARLAND",
	"SAFE",
	"SAFETY",
	"SAKURA",
	"SALE",
	"SALON",
	"SAMSUNG",
	"SANDVIK",
	"SANDVIKCOROMANT",
	"SANOFI",
	"SAP",
	"SAPO",
	"SARL",
	"SAS",
	"SAXO",
	"SB",
	"SBI",
	"SBS",
	"SC",
	"SCA",
	"SCB",
	"SCHAEFFLER",
	"SCHMIDT",
	"SCHOLARSHIPS",
	"SCHOOL",
	"SCHULE",
	"SCHWARZ",
	"SCIENCE",
	"SCOR",
	"SCOT",
	"SD",
	"SE",
	"SEAT",
	"SECURITY",
	"SEEK",
	"SELECT",
	"SENER",
	"SERVICES",
	"SEVEN",
	"SEW",
	"SEX",
	"SEXY",
	"SFR",
	"SG",
	"SH",
	"SHARP",
	"SHAW",
	"SHELL",
	"SHIA",
	"SHIKSHA",
	"SHOES",
	"SHOUJI",
	"SHOW",
	"SHRIRAM",
	"SI",
	"SINA",
	"SINGLES",
	"SITE",
	"SJ",
	"SK",
	"SKI",
	"SKIN",
	"SKY",
	"SKYPE",
	"SL",
	"SM",
	"SMILE",
	"SN",
	"SNCF",
	"SO",
	"SOCCER",
	"SOCIAL",
	"SOFTBANK",
	"SOFTWARE",
	"SOHU",
	"SOLAR",
	"SOLUTIONS",
	"SONG",
	"SONY",
	"SOY",
	"SPACE",
	"SPIEGEL",
	"SPOT",
	"SPREADBETTING",
	"SR",
	"SRL",
	"ST",
	"STADA",
	"STAR",
	"STARHUB",
	"STATEBANK",
	"STATEFARM",
	"STATOIL",
	"STC",
	"STCGROUP",
	"STOCKHOLM",
	"STORAGE",
	"STORE",
	"STREAM",
	"STUDIO",
	"STUDY",
	"STYLE",
	"SU",
	"SUCKS",
	"SUPPLIES",
	"SUPPLY",
	"SUPPORT",
	"SURF",
	"SURGERY",
	"SUZUKI",
	"SV",
	"SWATCH",
	"SWISS",
	"SX",
	"SY",
	"SYDNEY",
	"SYMANTEC",
	"SYSTEMS",
	"SZ",
	"TAB",
	"TAIPEI",
	"TALK",
	"TAOBAO",
	"TATAMOTORS",
	"TATAR",
	"TATTOO",
	"TAX",
	"TAXI",
	"TC",
	"TCI",
	"TD",
	"TEAM",
	"TECH",
	"TECHNOLOGY",
	"TEL",
	"TELECITY",
	"TELEFONICA",
	"TEMASEK",
	"TENNIS",
	"TEVA",
	"TF",
	"TG",
	"TH",
	"THD",
	"THEATER",
	"THEATRE",
	"TICKETS",
	"TIENDA",
	"TIFFANY",
	"TIPS",
	"TIRES",
	"TIROL",
	"TJ",
	"TK",
	"TL",
	"TM",
	"TMALL",
	"TN",
	"TO",
	"TODAY",
	"TOKYO",
	"TOOLS",
	"TOP",
	"TORAY",
	"TOSHIBA",
	"TOTAL",
	"TOURS",
	"TOWN",
	"TOYOTA",
	"TOYS",
	"TR",
	"TRADE",
	"TRADING",
	"TRAINING",
	"TRAVEL",
	"TRAVELERS",
	"TRAVELERSINSURANCE",
	"TRUST",
	"TRV",
	"TT",
	"TUBE",
	"TUI",
	"TUNES",
	"TUSHU",
	"TV",
	"TVS",
	"TW",
	"TZ",
	"UA",
	"UBS",
	"UG",
	"UK",
	"UNICOM",
	"UNIVERSITY",
	"UNO",
	"UOL",
	"US",
	"UY",
	"UZ",
	"VA",
	"VACATIONS",
	"VANA",
	"VC",
	"VE",
	"VEGAS",
	"VENTURES",
	"VERISIGN",
	"VERSICHERUNG",
	"VET",
	"VG",
	"VI",
	"VIAJES",
	"VIDEO",
	"VIG",
	"VIKING",
	"VILLAS",
	"VIN",
	"VIP",
	"VIRGIN",
	"VISION",
	"VISTA",
	"VISTAPRINT",
	"VIVA",
	"VLAANDEREN",
	"VN",
	"VODKA",
	"VOLKSWAGEN",
	"VOTE",
	"VOTING",
	"VOTO",
	"VOYAGE",
	"VU",
	"VUELOS",
	"WALES",
	"WALTER",
	"WANG",
	"WANGGOU",
	"WARMAN",
	"WATCH",
	"WATCHES",
	"WEATHER",
	"WEATHERCHANNEL",
	"WEBCAM",
	"WEBER",
	"WEBSITE",
	"WED",
	"WEDDING",
	"WEIBO",
	"WEIR",
	"WF",
	"WHOSWHO",
	"WIEN",
	"WIKI",
	"WILLIAMHILL",
	"WIN",
	"WINDOWS",
	"WINE",
	"WME",
	"WOLTERSKLUWER",
	"WORK",
	"WORKS",
	"WORLD",
	"WS",
	"WTC",
	"WTF",
	"XBOX",
	"XEROX",
	"XIHUAN",
	"XIN",
	"XN--11B4C3D",
	"XN--1CK2E1B",
	"XN--1QQW23A",
	"XN--30RR7Y",
	"XN--3BST00M",
	"XN--3DS443G",
	"XN--3E0B707E",
	"XN--3PXU8K",
	"XN--42C2D9A",
	"XN--45BRJ9C",
	"XN--45Q11C",
	"XN--4GBRIM",
	"XN--55QW42G",
	"XN--55QX5D",
	"XN--5TZM5G",
	"XN--6FRZ82G",
	"XN--6QQ986B3XL",
	"XN--80ADXHKS",
	"XN--80AO21A",
	"XN--80ASEHDB",
	"XN--80ASWG",
	"XN--8Y0A063A",
	"XN--90A3AC",
	"XN--90AIS",
	"XN--9DBQ2A",
	"XN--9ET52U",
	"XN--9KRT00A",
	"XN--B4W605FERD",
	"XN--BCK1B9A5DRE4C",
	"XN--C1AVG",
	"XN--C2BR7G",
	"XN--CCK2B3B",
	"XN--CG4BKI",
	"XN--CLCHC0EA0B2G2A9GCD",
	"XN--CZR694B",
	"XN--CZRS0T",
	"XN--CZRU2D",
	"XN--D1ACJ3B",
	"XN--D1ALF",
	"XN--E1A4C",
	"XN--ECKVDTC9D",
	"XN--EFVY88H",
	"XN--ESTV75G",
	"XN--FCT429K",
	"XN--FHBEI",
	"XN--FIQ228C5HS",
	"XN--FIQ64B",
	"XN--FIQS8S",
	"XN--FIQZ9S",
	"XN--FJQ720A",
	"XN--FLW351E",
	"XN--FPCRJ9C3D",
	"XN--FZC2C9E2C",
	"XN--G2XX48C",
	"XN--GCKR3F0F",
	"XN--GECRJ9C",
	"XN--H2BRJ9C",
	"XN--HXT814E",
	"XN--I1B6B1A6A2E",
	"XN--IMR513N",
	"XN--IO0A7I",
	"XN--J1AEF",
	"XN--J1AMH",
	"XN--J6W193G",
	"XN--JLQ61U9W7B",
	"XN--JVR189M",
	"XN--KCRX77D1X4A",
	"XN--KPRW13D",
	"XN--KPRY57D",
	"XN--KPU716F",
	"XN--KPUT3I",
	"XN--L1ACC",
	"XN--LGBBAT1AD8J",
	"XN--MGB9AWBF",
	"XN--MGBA3A3EJT",
	"XN--MGBA3A4F16A",
	"XN--MGBA7C0BBN0A",
	"XN--MGBAAM7A8H",
	"XN--MGBAB2BD",
	"XN--MGBAYH7GPA",
	"XN--MGBB9FBPOB",
	"XN--MGBBH1A71E",
	"XN--MGBC0A9AZCG",
	"XN--MGBCA7DZDO",
	"XN--MGBERP4A5D4AR",
	"XN--MGBPL2FH",
	"XN--MGBT3DHD",
	"XN--MGBTX2B",
	"XN--MGBX4CD0AB",
	"XN--MIX891F",
	"XN--MK1BU44C",
	"XN--MXTQ1M",
	"XN--NGBC5AZD",
	"XN--NGBE9E0A",
	"XN--NODE",
	"XN--NQV7F",
	"XN--NQV7FS00EMA",
	"XN--NYQY26A",
	"XN--O3CW4H",
	"XN--OGBPF8FL",
	"XN--P1ACF",
	"XN--P1AI",
	"XN--PBT977C",
	"XN--PGBS0DH",
	"XN--PSSY2U",
	"XN--Q9JYB4C",
	"XN--QCKA1PMC",
	"XN--QXAM",
	"XN--RHQV96G",
	"XN--ROVU88B",
	"XN--S9BRJ9C",
	"XN--SES554G",
	"XN--T60B56A",
	"XN--TCKWE",
	"XN--UNUP4Y",
	"XN--VERMGENSBERATER-CTB",
	"XN--VERMGENSBERATUNG-PWB",
	"XN--VHQUV",
	"XN--VUQ861B",
	"XN--W4R85EL8FHU5DNRA",
	"XN--WGBH1C",
	"XN--WGBL6A",
	"XN--XHQ521B",
	"XN--XKC2AL3HYE2A",
	"XN--XKC2DL3A5EE0H",
	"XN--Y9A3AQ",
	"XN--YFRO4I67O",
	"XN--YGBI2AMMX",
	"XN--ZFR164B",
	"XPERIA",
	"XXX",
	"XYZ",
	"YACHTS",
	"YAHOO",
	"YAMAXUN",
	"YANDEX",
	"YE",
	"YODOBASHI",
	"YOGA",
	"YOKOHAMA",
	"YOU",
	"YOUTUBE",
	"YT",
	"YUN",
	"ZA",
	"ZARA",
	"ZERO",
	"ZIP",
	"ZM",
	"ZONE",
	"ZUERICH",
	"ZW",


	"AB.CA",
	"AC.AE",
	"AC.AT",
	"AC.CN",
	"AC.CR",
	"AC.CY",
	"AC.FJ",
	"AC.GG",
	"AC.ID",
	"AC.IL",
	"AC.IM",
	"AC.IN",
	"AC.JE",
	"AC.JP",
	"AC.KR",
	"AC.NZ",
	"AC.PA",
	"AC.TH",
	"AC.UG",
	"AC.UK",
	"AC.YU",
	"AC.ZA",
	"AD.JP",
	"AH.CN",
	"ALDERNEY.GG",
	"ALT.ZA",
	"ART.BR",
	"ART.DO",
	"ARTS.CO",
	"ARTS.VE",
	"ASN.AU",
	"ASN.LV",
	"BBS.TR",
	"BC.CA",
	"BIB.VE",
	"BJ.CN",
	"CO.AT",
	"CO.AO",
	"CO.CK",
	"CO.CR",
	"CO.GG",
	"CO.HU",
	"CO.ID",
	"CO.IL",
	"CO.IM",
	"CO.IN",
	"CO.JE",
	"CO.JP",
	"CO.KR",
	"COM.AR",
	"COM.AU",
	"COM.AZ",
	"COM.BB",
	"COM.BM",
	"COM.BR",
	"COM.BS",
	"COM.CN",
	"COM.CO",
	"COM.CU",
	"COM.CY",
	"COM.DO",
	"COM.EC",
	"COM.EG",
	"COM.FJ",
	"COM.GE",
	"COM.GU",
	"COM.HK",
	"COM.JO",
	"COM.KH",
	"COM.LA",
	"COM.LB",
	"COM.LC",
	"COM.LV",
	"COM.LY",
	"COM.MM",
	"COM.MO",
	"COM.MT",
	"COM.MX",
	"COM.MY",
	"COM.NA",
	"COM.NC",
	"COM.NI",
	"COM.NP",
	"COM.PA",
	"COM.PE",
	"COM.PH",
	"COM.PL",
	"COM.PY",
	"COM.RU",
	"COM.SG",
	"COM.SH",
	"COM.SY",
	"COM.TN",
	"COM.TR",
	"COM.TW",
	"COM.UA",
	"COM.UY",
	"COM.VE",
	"CONF.AU",
	"CONF.LV",
	"CO.NZ",
	"COOP",
	"CO.AE",
	"CO.SV",
	"CO.TH",
	"CO.UG",
	"CO.UK",
	"CO.VE",
	"CO.VI",
	"CO.YU",
	"CO.ZA",
	"CQ.CN",
	"CSIRO.AU",
	"ED.CR",
	"EDU.BM",
	"EDU.AR",
	"EDU.CN",
	"EDU.CO",
	"EDU.DO",
	"EDU.EC",
	"EDU.EG",
	"EDU.GE",
	"EDU.GU",
	"EDU.JO",
	"EDU.LC",
	"EDU.LV",
	"EDU.MM",
	"EDU.MO",
	"EDU.MY",
	"EDUNET.TN",
	"EDU.PA",
	"EDU.PY",
	"EDU.SG",
	"EDU.SH",
	"EDU.TR",
	"EDU.TW",
	"EDU.UY",
	"EDU.VE",
	"EDU.YU",
	"EDU.ZA",
	"ENS.TN",
	"ERNET.IN",
	"ESP.BR",
	"ETC.BR",
	"EUN.EG",
	"FI.CR",
	"FIN.EC",
	"FIN.TN",
	"FIRM.CO",
	"FIRM.VE",
	"G12.BR",
	"GD.CN",
	"GEN.NZ",
	"GOB.PA",
	"GO.CR",
	"GO.ID",
	"GO.KR",
	"GO.TH",
	"GO.UG",
	"GOV.AE",
	"GOV.AR",
	"GOV.AU",
	"GOV.BM",
	"GOV.BR",
	"GOV.CN",
	"GOV.CO",
	"GOV.CY",
	"GOV.DO",
	"GOV.EC",
	"GOV.EG",
	"GOVE.TW",
	"GOV.FJ",
	"GOV.GE",
	"GOV.GG",
	"GOV.GU",
	"GOV.IL",
	"GOV.IM",
	"GOV.IN",
	"GOV.JE",
	"GOV.JO",
	"GOV.JP",
	"GOV.LB",
	"GOV.LC",
	"GOV.LV",
	"GOV.MM",
	"GOV.MO",
	"GOV.MY",
	"GOV.SG",
	"GOV.SH",
	"GOV.TN",
	"GOVT.NZ",
	"GOV.TR",
	"GOV.UA",
	"GOV.UK",
	"GOV.VE",
	"GOV.ZA",
	"GS.CN",
	"GUERNSEY.GG",
	"GX.CN",
	"GZ.CN",
	"HB.CN",
	"HE.CN",
	"HI.CN",
	"HK.CN",
	"HL.CN",
	"HN.CN",
	"ID.AU",
	"ID.FJ",
	"ID.LV",
	"IND.BR",
	"IND.GG",
	"IND.JE",
	"IND.TN",
	"INF.BR",
	"INFO.AU",
	"INFO.CO",
	"INFO.HU",
	"INFO.TN",
	"INFO.VE",
	"INT.CO",
	"INTL.TN",
	"INT.VE",
	"JERSEY.JE",
	"JL.CN",
	"JS.CN",
	"K12.EC",
	"K12.IL",
	"K12.TR",
	"LKD.CO.IM",
	"LN.CN",
	"LTD.GG",
	"LTD.JE",
	"LTD.UK",
	"MB.CA",
	"MED.EC",
	"MIL.BR",
	"MIL.CO",
	"MIL.DO",
	"MIL.EC",
	"MIL.GE",
	"MIL.GU",
	"MIL.ID",
	"MIL.LB",
	"MIL.LV",
	"MIL.PH",
	"MIL.SH",
	"MIL.TR",
	"MIL.VE",
	"MIL.ZA",
	"MO.CN",
	"MOD.UK",
	"MUNI.IL",
	"MUSEUM",
	"NAME",
	"NAT.TN",
	"NB.CA",
	"NET.AR",
	"NET.AU",
	"NET.AZ",
	"NET.BB",
	"NET.BM",
	"NET.BR",
	"NET.BS",
	"NET.CN",
	"NET.CU",
	"NET.CY",
	"NET.DO",
	"NET.EC",
	"NET.EG",
	"NET.GE",
	"NET.GG",
	"NET.GU",
	"NET.HK",
	"NET.ID",
	"NET.IL",
	"NET.IM",
	"NET.IN",
	"NET.JE",
	"NET.JO",
	"NET.JP",
	"NET.KH",
	"NET.LA",
	"NET.LB",
	"NET.LC",
	"NET.LV",
	"NET.LY",
	"NET.MM",
	"NET.MO",
	"NET.MT",
	"NET.MX",
	"NET.MY",
	"NET.NA",
	"NET.NC",
	"NET.NP",
	"NET.NZ",
	"NET.PA",
	"NET.PE",
	"NET.PH",
	"NET.PL",
	"NET.PY",
	"NET.RU",
	"NET.SG",
	"NET.SH",
	"NET.SY",
	"NET.TH",
	"NET.TN",
	"NET.TR",
	"NET.TW",
	"NET.UA",
	"NET.UK",
	"NET.UY",
	"NET.VE",
	"NET.VI",
	"NET.ZA",
	"NF.CA",
	"NGO.PH",
	"NGO.ZA",
	"NHS.UK",
	"NIC.IM",
	"NIC.IN",
	"NM.CN",
	"NM.KR",
	"NOM.CO",
	"NOM.VE",
	"NOM.ZA",
	"NS.CA",
	"NSK.SU",
	"NT.CA",
	"NUI.HU",
	"NX.CN",
	"ON.CA",
	"OR.CR",
	"ORG.AE",
	"ORG.AR",
	"ORG.AU",
	"ORG.AZ",
	"ORG.BB",
	"ORG.BM",
	"ORG.BR",
	"ORG.BS",
	"ORG.CN",
	"ORG.CO",
	"ORG.CU",
	"ORG.CY",
	"ORG.DO",
	"ORG.EC",
	"ORG.EG",
	"ORG.FJ",
	"ORG.GE",
	"ORG.GG",
	"ORG.GU",
	"ORG.HK",
	"ORG.HU",
	"ORG.IL",
	"ORG.IM",
	"ORG.JE",
	"ORG.JP",
	"ORG.KH",
	"ORG.LA",
	"ORG.LB",
	"ORG.LC",
	"ORG.LV",
	"ORG.LY",
	"ORG.MM",
	"ORG.MO",
	"ORG.MT",
	"ORG.MX",
	"ORG.MY",
	"ORG.NA",
	"ORG.NC",
	"ORG.NZ",
	"ORG.PA",
	"ORG.PE",
	"ORG.PH",
	"ORG.PL",
	"ORG.PY",
	"ORG.RU",
	"ORG.SG",
	"ORG.SH",
	"ORG.SY",
	"ORG.TN",
	"ORG.TR",
	"ORG.TW",
	"ORG.UK",
	"ORG.UY",
	"ORG.VE",
	"ORG.VI",
	"ORG.YU",
	"ORG.ZA",
	"OR.ID",
	"OR.KR",
	"OR.TH",
	"ORT.NP",
	"OR.UG",
	"OZ.AU",
	"PE.CA",
	"PLC.CO.IM",
	"PLC.UK",
	"POLICE.UK",
	"PRIV.HU",
	"PSI.BR",
	"PVT.GE",
	"QC.CA",
	"QH.CN",
	"REC.BR",
	"REC.CO",
	"REC.VE",
	"RE.KR",
	"RES.IN",
	"RNRT.TN",
	"RNS.TN",
	"RNU.TN",
	"SA.CR",
	"SARK.GG",
	"SC.CN",
	"SCH.GG",
	"SCH.JE",
	"SCHOOL.FJ",
	"SCHOOL.ZA",
	"SCH.UK",
	"SCI.EG",
	"SH.CN",
	"SK.CA",
	"SLD.PA",
	"SN.CN",
	"STORE.CO",
	"STORE.VE",
	"SX.CN",
	"TEC.VE",
	"TELEMEMO.AU",
	"TJ.CN",
	"TM.HU",
	"TMP.BR",
	"TM.ZA",
	"TOURISM.TN",
	"TW.CN",
	"WEB.CO",
	"WEB.DO",
	"WEB.VE",
	"WEB.ZA",
	"XJ.CN",
	"XZ.CN",
	"YK.CA",
	"YN.CN",
	"ZJ.CN"
};

	if ( ! s_isInitialized ) {
		// set up the hash table
		if ( ! s_table.set ( 8 , 0, sizeof(s_tlds)*2,NULL,0,false,0,
				     "tldtbl") ) 
			return log("build: Could not init table of TLDs.");
		// now add in all the stop words
		int32_t n = (int32_t)sizeof(s_tlds)/ sizeof(char *); 
		for ( int32_t i = 0 ; i < n ; i++ ) {
			const char      *d    = s_tlds[i];
			int32_t       dlen = gbstrlen ( d );
			int64_t  dh   = hash64Lower_a ( d , dlen );
			if ( ! s_table.addKey (&dh,NULL) )
				return log("build: dom table failed");
		}
		s_isInitialized = true;
	} 
	int64_t h = hash64Lower_a ( tld , tldLen ); // gbstrlen(tld));
	return s_table.isInTable ( &h );//getScoreFromTermId ( h );
}		

void resetDomains ( ) {
	s_table.reset();
}
