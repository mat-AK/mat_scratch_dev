<<<<<<< HEAD
#ifndef UNTITLED7_PENSETTINGS_H
#define UNTITLED7_PENSETTINGS_H

#include <iostream>
#include <string>
#include <vector>
#include <cmath>

enum PenProperty {
    PROP_COLOR,
    PROP_SATURATION,
    PROP_BRIGHTNESS
};
struct Color {
    int r, g, b;
    Color() : r(0), g(0), b(0) {}
    Color(int red, int green, int blue) : r(red), g(green), b(blue) {
        clamp();
    }
    void clamp() {
        if (r < 0) r = 0; if (r > 255) r = 255;
        if (g < 0) g = 0; if (g > 255) g = 255;
        if (b < 0) b = 0; if (b > 255) b = 255;
    }
    void toHSL(float& h, float& s, float& l) const {
        float rf = r / 255.0f;
        float gf = g / 255.0f;
        float bf = b / 255.0f;
        float maxVal = std::max(rf, std::max(gf, bf));
        float minVal = std::min(rf, std::min(gf, bf));
        float delta = maxVal - minVal;
        l = (maxVal + minVal) / 2.0f;
        if (delta == 0) {
            s = 0;
        } else {
            s = delta / (1 - std::abs(2 * l - 1));
        }
        if (delta == 0) {
            h = 0;
        } else if (maxVal == rf) {
            h = 60.0f * fmod(((gf - bf) / delta), 6);
        } else if (maxVal == gf) {
            h = 60.0f * (((bf - rf) / delta) + 2);
        } else {
            h = 60.0f * (((rf - gf) / delta) + 4);
        }

        if (h < 0) h += 360;
    }
    void fromHSL(float h, float s, float l) {
        float c = (1 - std::abs(2 * l - 1)) * s;
        float x = c * (1 - std::abs(fmod(h / 60.0f, 2) - 1));
        float m = l - c / 2;
        float rf, gf, bf;

        if (h < 60) { rf = c; gf = x; bf = 0; }
        else if (h < 120) { rf = x; gf = c; bf = 0; }
        else if (h < 180) { rf = 0; gf = c; bf = x; }
        else if (h < 240) { rf = 0; gf = x; bf = c; }
        else if (h < 300) { rf = x; gf = 0; bf = c; }
        else { rf = c; gf = 0; bf = x; }

        r = static_cast<int>((rf + m) * 255);
        g = static_cast<int>((gf + m) * 255);
        b = static_cast<int>((bf + m) * 255);

        clamp();
    }
    void setSaturation(float saturation) {
        float h, s, l;
        toHSL(h, s, l);
        s = std::max(0.0f, std::min(1.0f, saturation));
        fromHSL(h, s, l);
    }
    void setBrightness(float brightness) {
        float h, s, l;
        toHSL(h, s, l);
        l = std::max(0.0f, std::min(1.0f, brightness));
        fromHSL(h, s, l);
    }
    void changeSaturation(float delta) {
        float h, s, l;
        toHSL(h, s, l);
        s += delta;
        if (s < 0) s = 0;
        if (s > 1) s = 1;
        fromHSL(h, s, l);
    }
    void changeBrightness(float delta) {
        float h, s, l;
        toHSL(h, s, l);
        l += delta;
        if (l < 0) l = 0;
        if (l > 1) l = 1;
        fromHSL(h, s, l);
    }
};
void setPenColor(Color& color, int newR, int newG, int newB) {
    color = Color(newR, newG, newB);
    std::cout << "Color set to RGB(" << color.r << ","
              << color.g << "," << color.b << ")" << std::endl;
}
void setPenProperty(Color& color, PenProperty prop, float value) {
    switch(prop) {
        case PROP_COLOR:
            std::cout << "Use setPenColor for direct color selection" << std::endl;
            break;
        case PROP_SATURATION:
            color.setSaturation(value / 100.0f);
            std::cout << "Saturation set to " << value << "%" << std::endl;
            break;
        case PROP_BRIGHTNESS:
            color.setBrightness(value / 100.0f);
            std::cout << "Brightness set to " << value << "%" << std::endl;
            break;
    }
}
void changePenProperty(Color& color, PenProperty prop, float delta) {
    switch(prop) {
        case PROP_COLOR:
            std::cout << "Use color picker for color changes" << std::endl;
            break;
        case PROP_SATURATION:
            color.changeSaturation(delta / 100.0f);
            std::cout << "Saturation changed by " << delta << "%" << std::endl;
            break;
        case PROP_BRIGHTNESS:
            color.changeBrightness(delta / 100.0f);
            std::cout << "Brightness changed by " << delta << "%" << std::endl;
            break;
    }
}
Color pickColorFromWheel(int mouseX, int mouseY, int centerX, int centerY, int radius) {
    float dx = mouseX - centerX;
    float dy = mouseY - centerY;
    float distance = std::sqrt(dx*dx + dy*dy);

    if (distance > radius) {
        return Color(128, 128, 128);
    }
    float angle = std::atan2(dy, dx) * 180 / 3.14159f;
    if (angle < 0) angle += 360;
    float saturation = distance / radius;
    float brightness = 0.5f;

    Color result;
    float h = angle;
    float s = saturation;
    float l = brightness;

    float c = (1 - std::abs(2 * l - 1)) * s;
    float x = c * (1 - std::abs(fmod(h / 60.0f, 2) - 1));
    float m = l - c / 2;

    float rf, gf, bf;

    if (h < 60) { rf = c; gf = x; bf = 0; }
    else if (h < 120) { rf = x; gf = c; bf = 0; }
    else if (h < 180) { rf = 0; gf = c; bf = x; }
    else if (h < 240) { rf = 0; gf = x; bf = c; }
    else if (h < 300) { rf = x; gf = 0; bf = c; }
    else { rf = c; gf = 0; bf = x; }

    result.r = static_cast<int>((rf + m) * 255);
    result.g = static_cast<int>((gf + m) * 255);
    result.b = static_cast<int>((bf + m) * 255);
    result.clamp();

    return result;
}

#endif
=======
#ifndef PROJECT_PENSETTINGS_H
#define PROJECT_PENSETTINGS_H

#include <iostream>
#include <vector>


#endif //PROJECT_PENSETTINGS_H
>>>>>>> b095a9fa5a4b64bbcdfac393e50073f302f95b3c
