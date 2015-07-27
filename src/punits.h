#ifndef PUNITS
#define PUNITS

#include <QObject>

class PUnits : public QObject {
    Q_OBJECT
    Q_PROPERTY(float scale READ scale WRITE setContentScaleFactor NOTIFY contentScaleFactorChanged)
    Q_PROPERTY(float fontScale READ getFontScaleFactor WRITE setFontScaleFactor NOTIFY fontScaleChanged)
    Q_PROPERTY(float dpi READ getDpi WRITE setDpi NOTIFY dpiChanged)

public:
    explicit PUnits(QObject* parent = 0) : QObject(parent), contentScaleFactor(1), fontScaleFactor(1.5), dpi(160) {}

    static PUnits& instance() {
        static PUnits instance;
        return instance;
    }

    float scale() { return contentScaleFactor; }
    void setContentScaleFactor(float new_value) { contentScaleFactor = new_value; emit contentScaleFactorChanged(); }

    float getFontScaleFactor() const { return fontScaleFactor; }
    void setFontScaleFactor(float value) { fontScaleFactor = value; emit fontScaleChanged(); }

    float getDpi() const { return dpi; }
    void setDpi(float value) { dpi = value; emit dpiChanged(); }
public slots:
    float dp(float value) { return value * scale(); }
    float sp(float value) { return value * getFontScaleFactor(); }

signals:
    void contentScaleFactorChanged();
    void dpiChanged();
    void fontScaleChanged();

private:
    float contentScaleFactor;
    float fontScaleFactor;
    float dpi;
};

#endif // PUNITS
