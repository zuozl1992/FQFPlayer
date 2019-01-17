#include "colorquickimageprovider.h"

ColorQuickImageProvider::ColorQuickImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{

}

QPixmap ColorQuickImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    int width = 1;
    int height = 1;

    if (size)
        *size = QSize(width, height);
    QPixmap pixmap(requestedSize.width() > 0 ? requestedSize.width() : width,
                 requestedSize.height() > 0 ? requestedSize.height() : height);
    pixmap.fill(QColor(id).rgba());

    return pixmap;
}
