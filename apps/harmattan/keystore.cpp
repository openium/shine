#include "keystore.h"

KeyStore::KeyStore(QObject *parent): QObject(parent),
    m_settings(QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + "/.config/shine/shine.conf", QSettings::IniFormat)
{
}

QString KeyStore::apiKey() const
{
    qDebug() << "reading from" << m_settings.fileName();
    return m_settings.value("apiKey").toString();
}

void KeyStore::setApiKey(const QString &apiKey)
{
    m_settings.setValue("apiKey", apiKey);
    emit apiKeyChanged();
}
