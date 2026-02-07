// Mapping of iOS bundle identifiers to human-readable app names.
// Only includes apps that typically send notifications.
#ifndef KNOWN_APPS_H
#define KNOWN_APPS_H

#include <Arduino.h>

struct AppNameEntry
{
    const char *bundleId;
    const char *appName;
};

static const AppNameEntry kAppNames[] = {
    // ===== APPLE SYSTEM APPS =====
    {"com.apple.MobileSMS", "Messages"},
    {"com.apple.mobilemail", "Mail"},
    {"com.apple.mobilephone", "Phone"},
    {"com.apple.mobilecal", "Calendar"},
    {"com.apple.reminders", "Reminders"},
    {"com.apple.facetime", "FaceTime"},
    {"com.apple.news", "News"},
    {"com.apple.mobiletimer", "Clock"},
    {"com.apple.findmy", "Find My"},
    {"com.apple.Health", "Health"},
    {"com.apple.Fitness", "Fitness"},
    {"com.apple.mobilenotes", "Notes"},
    {"com.apple.Home", "Home"},
    {"com.apple.shortcuts", "Shortcuts"},
    {"com.apple.Music", "Music"},
    {"com.apple.podcasts", "Podcasts"},
    {"com.apple.weather", "Weather"},

    // ===== SOCIAL MEDIA =====
    {"com.facebook.Facebook", "Facebook"},
    {"com.burbn.instagram", "Instagram"},
    {"com.atebits.Tweetie2", "Twitter"},
    {"com.twitter.twitter-iphone", "X"},
    {"com.toyopagroup.picaboo", "Snapchat"},
    {"com.linkedin.LinkedIn", "LinkedIn"},
    {"com.zhiliaoapp.musically", "TikTok"},
    {"com.pinterest.iphone", "Pinterest"},
    {"com.reddit.Reddit", "Reddit"},
    {"com.tumblr.tumblr", "Tumblr"},
    {"flipboard.app", "Flipboard"},
    {"com.facebook.Pages", "FB Pages"},
    {"com.facebook.groups", "FB Groups"},

    // ===== MESSAGING APPS =====
    {"net.whatsapp.WhatsApp", "WhatsApp"},
    {"ph.telegra.Telegraph", "Telegram"},
    {"com.facebook.Messenger", "Messenger"},
    {"org.thoughtcrime.securesms", "Signal"},
    {"com.skype.skype", "Skype"},
    {"com.viber", "Viber"},
    {"jp.naver.line", "LINE"},
    {"com.kakao.talk", "KakaoTalk"},
    {"com.discord", "Discord"},
    {"com.tinyspeck.chatlyio", "Slack"},
    {"com.google.hangouts", "Hangouts"},
    {"com.imo.iphone", "imo"},
    {"net.whatsapp.WhatsAppSMB", "WhatsApp Business"},
    {"com.tencent.xin", "WeChat"},
    {"com.groupme.GroupMe", "GroupMe"},

    // ===== EMAIL APPS =====
    {"com.google.Gmail", "Gmail"},
    {"com.microsoft.Office.Outlook", "Outlook"},
    {"com.readdle.smartemail", "Spark"},
    {"protonmail.ios.ProtonMail", "ProtonMail"},
    {"com.readdle.mail", "Edison Mail"},
    {"com.yahoo.Aerodrome", "Yahoo Mail"},
    {"com.aol.mail", "AOL Mail"},
    {"com.easilydo.mail", "Edison"},

    // ===== PRODUCTIVITY & WORK =====
    {"com.microsoft.teams", "Teams"},
    {"com.zoom.videomeetings", "Zoom"},
    {"us.zoom.videomeetings", "Zoom"},
    {"com.google.meet", "Google Meet"},
    {"com.webex.meetingmanager", "Webex"},
    {"com.notion.iOS.Notion", "Notion"},
    {"com.evernote.iPhone.Evernote", "Evernote"},
    {"com.trello.trello", "Trello"},
    {"com.asana.app", "Asana"},
    {"com.monday.monday", "Monday"},
    {"com.atlassian.jira.core", "Jira"},
    {"com.google.Drive", "Google Drive"},
    {"com.getdropbox.Dropbox", "Dropbox"},
    {"com.microsoft.onenote", "OneNote"},
    {"com.todoist", "Todoist"},
    {"com.any.do", "Any.do"},

    // ===== BANKING & FINANCE =====
    {"com.paypal.PPClient", "PayPal"},
    {"com.3mosquitos.MercadoLibre", "MercadoLibre"},
    {"com.3mosquitos.mercadolibre", "MercadoLibre"},
    {"com.mercadopago.MercadoPago", "MercadoPago"},
    {"com.mercadopago.mercadopago", "MercadoPago"},
    {"ar.com.santander.rio.mbanking", "Santander Rio"},
    {"com.squareup.cash", "Cash App"},
    {"com.coinbase.consumer", "Coinbase"},
    {"com.venmo", "Venmo"},
    {"com.bankofamerica.mobile", "Bank of America"},
    {"com.chase.mobile", "Chase"},
    {"com.wellsfargo.mobile", "Wells Fargo"},
    {"com.mint", "Mint"},
    {"com.robinhood.release.Robinhood", "Robinhood"},
    {"com.citi.citimobile", "Citi Mobile"},
    {"com.usaa.mobile.ios.usaa", "USAA"},
    {"com.zellepay.zelle", "Zelle"},
    {"com.revolut.revolut", "Revolut"},
    {"com.plaid.consumer", "Plaid"},

    // ===== DELIVERY & FOOD =====
    {"com.ubereats.eats", "Uber Eats"},
    {"com.grubhub.client", "Grubhub"},
    {"com.dd.doordash", "DoorDash"},
    {"com.postmates.ios.consumer", "Postmates"},
    {"com.yelp.yelpiphone", "Yelp"},
    {"com.opentable.opentable", "OpenTable"},
    {"com.instacart.client", "Instacart"},
    {"com.starbucks.iphone", "Starbucks"},
    {"com.mcdonalds.mobileapp", "McDonald's"},
    {"com.dominos.Dominos", "Dominos"},

    // ===== ENTERTAINMENT & MEDIA =====
    {"com.google.ios.youtube", "YouTube"},
    {"com.netflix.Netflix", "Netflix"},
    {"com.spotify.client", "Spotify"},
    {"com.google.android.apps.youtube.music", "YouTube Music"},
    {"com.pandora", "Pandora"},
    {"com.hulu.plus", "Hulu"},
    {"com.disney.disneyplus", "Disney+"},
    {"com.hbo.hbonow", "HBO Max"},
    {"com.hbo.hbomax", "HBO Max"},
    {"com.amazon.primeVideo", "Prime Video"},
    {"tv.twitch", "Twitch"},
    {"com.soundcloud.TouchApp", "SoundCloud"},
    {"com.deezer.Deezer", "Deezer"},
    {"fm.last.LastFm", "Last.fm"},
    {"com.applemusic", "Apple Music"},

    // ===== SHOPPING =====
    {"com.amazon.Amazon", "Amazon"},
    {"com.ebay.iphone", "eBay"},
    {"com.etsy.etsyforios", "Etsy"},
    {"com.target.mobile", "Target"},
    {"com.walmart.shopapp", "Walmart"},
    {"com.contextoptional.AmazonPrimeNow", "Amazon Prime"},
    {"com.shopify.arrive", "Shopify"},
    {"com.wish.consumer", "Wish"},
    {"com.alibaba.aliexpress", "AliExpress"},
    {"com.shein.shein", "SHEIN"},

    // ===== TRAVEL & NAVIGATION =====
    {"com.google.Maps", "Google Maps"},
    {"com.waze.iphone", "Waze"},
    {"com.ubercab.UberClient", "Uber"},
    {"com.lyft.lyft", "Lyft"},
    {"com.airbnb.app", "Airbnb"},
    {"com.booking.booking", "Booking.com"},
    {"com.tripadvisor.TripAdvisor", "TripAdvisor"},
    {"com.expedia.bookings", "Expedia"},
    {"com.hotels.hotelsdotcom", "Hotels.com"},
    {"com.delta.mobile.iphone", "Delta"},
    {"com.aa.americanairlines", "American Airlines"},
    {"com.southwest.mobile", "Southwest"},
    {"com.united.mobile.iphone", "United"},

    // ===== NEWS & READING =====
    {"com.nytimes.NYTimes", "NY Times"},
    {"com.cnn.iphone", "CNN"},
    {"com.bbcnews.international", "BBC News"},
    {"com.medium.reader", "Medium"},
    {"com.theguardian", "The Guardian"},
    {"com.washingtonpost.iphone", "Washington Post"},
    {"com.wsj.WSJMobile", "Wall Street Journal"},
    {"com.usatoday.iphone.iphone", "USA Today"},
    {"com.bloomberg.Bloomberg", "Bloomberg"},
    {"com.google.news", "Google News"},
    {"com.apple.news", "Apple News"},

    // ===== GAMING =====
    {"com.supercell.magic", "Clash of Clans"},
    {"com.supercell.laser", "Clash Royale"},
    {"com.supercell.brawlstars", "Brawl Stars"},
    {"com.roblox.robloxmobile", "Roblox"},
    {"com.epicgames.fortnite", "Fortnite"},
    {"com.miHoYo.GenshinImpact", "Genshin Impact"},
    {"com.king.candycrushsaga", "Candy Crush"},
    {"com.playrix.homescapes", "Homescapes"},
    {"com.ea.ios.apexlegends", "Apex Legends"},
    {"com.ea.ios.fifamobile", "FIFA Mobile"},

    // ===== FITNESS & HEALTH =====
    {"com.nike.nikeplus-gps", "Nike Run Club"},
    {"com.strava.stravaride", "Strava"},
    {"com.myfitnesspal.mfp", "MyFitnessPal"},
    {"com.peloton.peloton", "Peloton"},
    {"com.calm.app", "Calm"},
    {"com.headspace.headspace", "Headspace"},
    {"com.fitbit.FitbitMobile", "Fitbit"},
    {"com.samsung.health", "Samsung Health"},
    {"com.noom.noom", "Noom"},

    // ===== DATING =====
    {"com.cardify.tinder", "Tinder"},
    {"com.bumble.Bumble", "Bumble"},
    {"com.match.Match", "Match"},
    {"com.hinge.hinge", "Hinge"},
    {"com.okcupid.OkCupid", "OkCupid"},
    {"com.pof.pof", "Plenty of Fish"},
    {"com.coffee.match.bagel", "Coffee Meets Bagel"},

    // ===== UTILITIES & OTHER =====
    {"com.shazam.Shazam", "Shazam"},
    {"com.babbel.mobile", "Babbel"},
    {"com.google.chrome.ios", "Chrome"},
    {"com.brave.ios.browser", "Brave"},
    {"com.duckduckgo.mobile.ios", "DuckDuckGo"},
    {"com.getpocket.pocket", "Pocket"},
    {"com.contextlogic.Wish", "Wish"},
    {"nextdoor.nextdoor", "Nextdoor"},
    {"com.offerup.offerup", "OfferUp"},
    {"com.craigslist.craigslistmobile", "Craigslist"},
    {"com.openai.chat", "ChatGPT"},
    {"com.ring.ring", "Ring"},
    {"com.nestlabs.jasper", "Nest"},
    {"com.philips.hue", "Philips Hue"},
    {"com.duolingo.DuolingoMobile", "Duolingo"},
};

static inline String normalizeBundleId(String value)
{
    value.trim();
    value.toLowerCase();

    if (value.startsWith("\"") && value.endsWith("\"") && value.length() >= 2)
    {
        value = value.substring(1, value.length() - 1);
        value.trim();
    }
    return value;
}

static inline bool bundleIdMatches(const String &normalizedIncoming, const char *knownBundleId)
{
    String normalizedKnown = normalizeBundleId(String(knownBundleId));
    if (normalizedIncoming == normalizedKnown)
    {
        return true;
    }

    // Some ANCS payloads may include prefixes/suffixes around bundle id.
    if (normalizedIncoming.endsWith(normalizedKnown))
    {
        return true;
    }
    return false;
}

static inline String getAppName(const String &bundleId)
{
    String normalizedIncoming = normalizeBundleId(bundleId);

    for (size_t i = 0; i < (sizeof(kAppNames) / sizeof(kAppNames[0])); ++i)
    {
        if (bundleIdMatches(normalizedIncoming, kAppNames[i].bundleId))
        {
            return kAppNames[i].appName;
        }
    }
    return bundleId;
}

#endif
