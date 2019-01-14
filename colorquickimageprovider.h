#ifndef COLORQUICKIMAGEPROVIDER_H
#define COLORQUICKIMAGEPROVIDER_H

#include <QQuickImageProvider>

class ColorQuickImageProvider : public QQuickImageProvider
{
public:
    ColorQuickImageProvider();
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
};

#endif // COLORQUICKIMAGEPROVIDER_H
