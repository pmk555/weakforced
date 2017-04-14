#pragma once
#include <string>
#include <GeoIP.h>
#include <GeoIPCity.h>
#include "lock.hh"
#include "iputils.hh"

enum class WFGeoIPDBType : uint32_t { GEOIP_NONE=0x00, GEOIP_COUNTRY=0x01, GEOIP_CITY=0x02, GEOIP_ISP=0x04, GEOIP_COUNTRY_V6=0x08, GEOIP_CITY_V6=0x10, GEOIP_ISP_V6=0x20 };

constexpr enum WFGeoIPDBType operator |( const enum WFGeoIPDBType selfValue, const enum WFGeoIPDBType inValue )
{
    return (enum WFGeoIPDBType)(uint32_t(selfValue) | uint32_t(inValue));
}

constexpr bool operator &( const enum WFGeoIPDBType selfValue, const enum WFGeoIPDBType inValue )
{
    return (uint32_t(selfValue) & uint32_t(inValue));
}

struct WFGeoIPRecord {
  std::string country_code;
  std::string country_name;
  std::string region;
  std::string city;
  std::string postal_code;
  std::string continent_code;
  float	      latitude;
  float	      longitude;
};

class WFGeoIPDB
{
public:
  WFGeoIPDB()
  {
  }

  ~WFGeoIPDB()
  {
    if (gi_v4) {
      GeoIP_delete(gi_v4);
    }
    if (gi_v6) {
      GeoIP_delete(gi_v6);
    }
    if (gi_city_v4) {
      GeoIP_delete(gi_city_v4);
    }
    if (gi_city_v6) {
      GeoIP_delete(gi_city_v6);
    }
    if (gi_isp_v4) {
      GeoIP_delete(gi_isp_v4);
    }
    if (gi_isp_v6) {
      GeoIP_delete(gi_isp_v6);
    }
  }

  // Only load it if someone wants to use GeoIP, otherwise it's a waste of RAM
  void initGeoIPDB(WFGeoIPDBType db_types); // pass these as flags
  // This will lookup in either the v4 or v6 GeoIP DB, depending on what address is
  std::string lookupCountry(const ComboAddress& address) const;
  std::string lookupISP(const ComboAddress& address) const;
  WFGeoIPRecord lookupCity(const ComboAddress& address) const;
  void reload(); // Reload any opened DBs from original files
protected:
  GeoIP* openGeoIPDB(GeoIPDBTypes db_type, const std::string& name);
private:
  // GeoIPDB seems to have different DBs for v4 and v6 addresses, hence two DBs
  GeoIP *gi_v4 = NULL;
  GeoIP *gi_v6 = NULL;
  GeoIP *gi_city_v4 = NULL;
  GeoIP *gi_city_v6 = NULL;
  GeoIP *gi_isp_v4 = NULL;
  GeoIP *gi_isp_v6 = NULL;
  mutable pthread_rwlock_t gi_rwlock = PTHREAD_RWLOCK_INITIALIZER;
};

extern WFGeoIPDB g_wfgeodb;
