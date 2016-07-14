#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QFile>
#include <QSet>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <ovlnet/buckets.hh>


class SubSetting: public QObject
{
  Q_OBJECT

protected:
  SubSetting(const QJsonValue &config, QObject *parent=0);

public:
  virtual ~SubSetting();

  virtual QJsonValue serialize() const = 0;

signals:
  void modified();
};


class NodeIdList : public SubSetting
{
  Q_OBJECT

public:
  typedef QSet<Identifier>::iterator iterator;
  typedef QSet<Identifier>::const_iterator const_iterator;

public:
  NodeIdList(const QJsonValue &value, QObject *parent=0);

  bool contains(const Identifier &id) const;
  void clear();
  void insert(const Identifier &id);

  iterator begin();
  const_iterator begin() const;
  iterator end();
  const_iterator end() const;

  QJsonValue serialize() const;

protected:
  QSet<Identifier> _list;
};


/** Holds the settings for the socks proxy service. */
class SocksServiceSettings: public SubSetting
{
  Q_OBJECT

public:
  /** Constructs the settings from the given JSON representation. */
  SocksServiceSettings(const QJsonValue &value, QObject *parent=0);

  bool enabled() const;
  void enable(bool enabled);

  bool allowBuddies() const;
  void setAllowBuddies(bool allow);

  bool allowWhiteListed() const;
  void setAllowWhiteListed(bool allow);

  NodeIdList &whitelist();
  const NodeIdList &whitelist() const;

  QJsonValue serialize() const;

protected:
  bool _enabled;
  bool _allowBuddies;
  bool _allowWhitelist;
  NodeIdList *_whitelist;
};


class UPNPSettings: public SubSetting
{
  Q_OBJECT

public:
  UPNPSettings(const QJsonValue &value, QObject *parent=0);

  bool enabled() const;
  void enable(bool enabled);

  uint16_t externalPort() const;
  void setExternalPort(uint16_t port);

  QJsonValue serialize() const;

protected:
  bool _enabled;
  uint16_t _externalPort;
};


/** Implements a persistent settings object, collecting the options of several modules and
 * services and keep them in a single file. */
class Settings : public QObject
{
  Q_OBJECT

public:
  /** Loads the settings from the given file (or initializes them with the default ones). */
  explicit Settings(const QString &filename, QObject *parent = 0);

  /** Returns a weak reference to the socks service settings. */
  SocksServiceSettings &socksServiceSettings();
  /** Returns a weak reference to the UPNP settings. */
  UPNPSettings &upnpSettings();

public slots:
  /** Save the current settings into the file give to the constructor. */
  void save();

protected:
  /** The settings file. */
  QFile _file;
  /** Settings for the socks proxy service. */
  SocksServiceSettings *_socksServiceSettings;
  /** Settings for the UPNP service. */
  UPNPSettings *_upnpSettings;
};

#endif // SETTINGS_H
