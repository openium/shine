#include "discovery.h"

#include <QDebug>
#include <QTimer>

// Discovery timeout in seconds
const unsigned int DISCOVERY_TIMEOUT = 3;

Discovery::Discovery(QObject *parent) :
    QUdpSocket(parent),
    m_timeout(new QTimer(this))
{
    quint16 port = 1900;
    unsigned int tries = 0;
    const unsigned int maxtries = 3;

    while (!bind(port++)) {
        if (++tries == maxtries) {
            emit error();
            return;
        }
    }

    connect(this, &Discovery::readyRead, this, &Discovery::onReadyRead);

    m_timeout->setSingleShot(true);
    connect(m_timeout, &QTimer::timeout, this, &Discovery::onTimeout);
}

void Discovery::findBridges()
{
    m_timeout->stop();
    m_reportedBridges.clear();

    QString b("M-SEARCH * HTTP/1.1\r\n"
              "HOST: 239.255.255.250:1900\r\n"
              "MAN: \"ssdp:discover\"\r\n"
              "MX: %1\r\n"
              "ST: libhue:idl\r\n");
    b.arg(DISCOVERY_TIMEOUT);

    m_timeout->start(DISCOVERY_TIMEOUT * 1000);
    if (writeDatagram(b.toUtf8(), QHostAddress("239.255.255.250"), 1900) < 0) {
        emit error();
    }
}

void Discovery::onTimeout()
{
    if (m_reportedBridges.isEmpty())
        emit noBridgesFound();
}

void Discovery::onReadyRead()
{
    while (hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        if (!m_reportedBridges.contains(sender)) {
            m_reportedBridges << sender;
            emit foundBridge(sender);
        }
    }
}
