#include "settings.hh"
#include <ovlnet/logger.hh>
#include <QJsonArray>


/* ********************************************************************************************* *
 * Implementation of SubSettings
 * ********************************************************************************************* */
SubSetting::SubSetting(const QJsonValue &config, QObject *parent)
  : QObject(parent)
{
  // pass...
}

SubSetting::~SubSetting() {
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of Settings
 * ********************************************************************************************* */
Settings::Settings(const QString &filename, QObject *parent)
  : QObject(parent), _file(filename), _socksServiceSettings(0), _upnpSettings(0)
{
  if (!_file.open(QIODevice::ReadOnly)) { return; }
  logDebug() << "Settings: Load client settings from " << filename;
  QJsonDocument doc = QJsonDocument::fromJson(_file.readAll());
  _file.close();

  if (! doc.isObject())
    return;

  // Socks service settings
  _socksServiceSettings = new SocksServiceSettings(doc.object().value("socks_service"), this);
  connect(_socksServiceSettings, SIGNAL(modified()), this, SLOT(save()));
  // UPNP settings
  _upnpSettings = new UPNPSettings(doc.object().value("upnp"), this);
  connect(_upnpSettings, SIGNAL(modified()), this, SLOT(save()));
}

void
Settings::save() {
  if (! _file.open(QIODevice::WriteOnly)) {
    logWarning() << "Settings: Cannot save settings to " << _file.fileName()
                 << ": " << _file.errorString();
    return;
  }

  QJsonObject obj;
  obj.insert("socks_service", _socksServiceSettings->serialize());
  QJsonDocument doc(obj);
  _file.write(doc.toJson());
  _file.close();
}

SocksServiceSettings &
Settings::socksServiceSettings() {
  return *_socksServiceSettings;
}

UPNPSettings &
Settings::upnpSettings() {
  return *_upnpSettings;
}


/* ********************************************************************************************* *
 * Implementation of SocksServiceSettings
 * ********************************************************************************************* */
SocksServiceSettings::SocksServiceSettings(const QJsonValue &value, QObject *parent)
  : SubSetting(value, parent), _enabled(false), _allowBuddies(false), _allowWhitelist(false),
    _whitelist(0)
{
  if (! value.isObject())
    return;

  QJsonObject object = value.toObject();
  // Get settings
  _enabled = object.value("enabled").toBool(false);
  _allowBuddies = object.value("allow_buddies").toBool(false);
  _allowWhitelist = object.value("allow_whitelist").toBool(false);
  _whitelist = new NodeIdList(object.value("whitelist"), this);
  // Forward modified signal
  connect(_whitelist, SIGNAL(modified()), this, SIGNAL(modified()));
}

bool
SocksServiceSettings::enabled() const {
  return _enabled;
}

void
SocksServiceSettings::enable(bool enabled) {
  _enabled = enabled;
  emit modified();
}

bool
SocksServiceSettings::allowBuddies() const {
  return _allowBuddies;
}

void
SocksServiceSettings::setAllowBuddies(bool allow) {
  _allowBuddies = allow;
  emit modified();
}

bool
SocksServiceSettings::allowWhiteListed() const {
  return _allowWhitelist;
}

void
SocksServiceSettings::setAllowWhiteListed(bool allow) {
  _allowWhitelist = allow;
  emit modified();
}

const NodeIdList &
SocksServiceSettings::whitelist() const {
  return *_whitelist;
}

NodeIdList &
SocksServiceSettings::whitelist() {
  return *_whitelist;
}

QJsonValue
SocksServiceSettings::serialize() const {
  QJsonObject obj;
  obj.insert("enabled", _enabled);
  obj.insert("allow_buddies", _allowBuddies);
  obj.insert("allow_whitelist", _allowWhitelist);
  obj.insert("whitelist", _whitelist->serialize());
  return obj;
}


/* ********************************************************************************************* *
 * Implementation of UPNPSettings
 * ********************************************************************************************* */
UPNPSettings::UPNPSettings(const QJsonValue &value, QObject *parent)
  : SubSetting(value, parent), _enabled(false), _externalPort(7742)
{
  if (! value.isObject())
    return;
  QJsonObject obj = value.toObject();
  if (obj.contains("enabled"))
    _enabled = obj.value("enabled").toBool(_enabled);
  if (obj.contains("external-port"))
    _externalPort = obj.value("external-port").toInt(_externalPort);
}

bool
UPNPSettings::enabled() const {
  return _enabled;
}

void
UPNPSettings::enable(bool enabled) {
  _enabled = enabled;
  emit modified();
}

uint16_t
UPNPSettings::externalPort() const {
  return _externalPort;
}

void
UPNPSettings::setExternalPort(uint16_t port) {
  if (_externalPort == port)
    return;
  _externalPort = port;
  emit modified();
}

QJsonValue
UPNPSettings::serialize() const {
  QJsonObject obj;
  obj.insert("enabled", _enabled);
  obj.insert("external-port", _externalPort);
  return obj;
}


/* ********************************************************************************************* *
 * Implementation of SocksServiceWhiteList
 * ********************************************************************************************* */
NodeIdList::NodeIdList(const QJsonValue &value, QObject *parent)
  : SubSetting(value, parent), _list()
{
  if (! value.isArray())
    return;

  QJsonArray lst = value.toArray();
  for (QJsonArray::const_iterator item = lst.begin(); item != lst.end(); item++) {
    if ((*item).isString()) {
      _list.insert(Identifier::fromBase32((*item).toString()));
    }
  }
}

bool
NodeIdList::contains(const Identifier &id) const {
  return _list.contains(id);
}

void
NodeIdList::clear() {
  _list.clear();
  emit modified();
}

void
NodeIdList::insert(const Identifier &id) {
  if (_list.contains(id))
    return;
  _list.insert(id);
  emit modified();
}

NodeIdList::iterator
NodeIdList::begin() {
  return _list.begin();
}

NodeIdList::iterator
NodeIdList::end() {
  return _list.end();
}

NodeIdList::const_iterator
NodeIdList::begin() const {
  return _list.begin();
}

NodeIdList::const_iterator
NodeIdList::end() const {
  return _list.end();
}

QJsonValue
NodeIdList::serialize() const {
  QJsonArray lst;
  QSet<Identifier>::const_iterator item = _list.begin();
  for (; item != _list.end(); item++) {
    lst.append(item->toBase32());
  }
  return lst;
}
