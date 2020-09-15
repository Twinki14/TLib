#ifndef PAINTBITMAP_HPP_INCLUDED
#define PAINTBITMAP_HPP_INCLUDED

#include <Core/Paint.hpp>
#include <fstream>
#include <algorithm>

class PaintBitmap
{
private:

    static std::string Base64Decode(const std::string& StringIn)
    {
        static const char* Base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::string StringOut = "";
        unsigned char CharArray3[3];
        unsigned char CharArray4[4];
        int Len = StringIn.size();
        int J = 0, K = 0;
        auto IsValid = [] (char C)
        {
            return (((isalnum(C)) || (C == '+') || (C == '/')) && (C != '='));
        };
        for (int I = 0; I < Len; I++)
        {
            if (!IsValid(StringIn[I]))
                break;
            CharArray4[J++] = StringIn[I];
            if (J ==4)
            {
                for (J = 0; J < 4; J++)
                    CharArray4[J] = std::find(&Base64Chars[0], &Base64Chars[63], CharArray4[J]) - &Base64Chars[0];
                CharArray3[0] = (CharArray4[0] << 2) + ((CharArray4[1] & 0x30) >> 4);
                CharArray3[1] = ((CharArray4[1] & 0xf) << 4) + ((CharArray4[2] & 0x3c) >> 2);
                CharArray3[2] = ((CharArray4[2] & 0x3) << 6) + CharArray4[3];
                for (J = 0; (J < 3); J++)
                    StringOut += CharArray3[J];
                J = 0;
            }
        }

        if (J)
        {
            for (K = J; K <4; K++)
                CharArray4[K] = 0;
            for (K = 0; K <4; K++)
                CharArray4[K] = std::find(&Base64Chars[0], &Base64Chars[63], CharArray4[K]) - &Base64Chars[0];
            CharArray3[0] = (CharArray4[0] << 2) + ((CharArray4[1] & 0x30) >> 4);
            CharArray3[1] = ((CharArray4[1] & 0xf) << 4) + ((CharArray4[2] & 0x3c) >> 2);
            CharArray3[2] = ((CharArray4[2] & 0x3) << 6) + CharArray4[3];
            for (K = 0; (K < J - 1); K++)
                StringOut += CharArray3[K];
        }
        return StringOut;
    }

    static std::string Base64Encode(const std::string& StringIn)
    {
        static const char* Base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::string StringOut = "";
        unsigned char CharArray3[3];
        unsigned char CharArray4[4];
        int Len = StringIn.size();
        int J = 0, K = 0;
        for (auto I = 0; I < Len; I++)
        {
            CharArray3[J++] = StringIn[I];
            if (J == 3)
            {
                CharArray4[0] = ((CharArray3[0] & 0xfc) >> 2);
                CharArray4[1] = ((CharArray3[0] & 0x03) << 4) + ((CharArray3[1] & 0xf0) >> 4);
                CharArray4[2] = ((CharArray3[1] & 0x0f) << 2) + ((CharArray3[2] & 0xc0) >> 6);
                CharArray4[3] = (CharArray3[2] & 0x3f);
                for (J = 0; J < 4; J++)
                    StringOut += Base64Chars[CharArray4[J]];
                J = 0;
            }

        }
        if (J)
        {
            for (K = J; K < 3; K++)
                CharArray3[K] = '\0';
            CharArray4[0] = ((CharArray3[0] & 0xfc) >> 2);
            CharArray4[1] = (((CharArray3[0] & 0x03) << 4) + ((CharArray3[1] & 0xf0) >> 4));
            CharArray4[2] = (((CharArray3[1] & 0x0f) << 2) + ((CharArray3[2] & 0xc0) >> 6));
            CharArray4[3] = (CharArray3[2] & 0x3f);
            for (K = 0; K <= J; K++)
                StringOut += Base64Chars[CharArray4[K]];
            while((J++) < 3)
                StringOut += '=';
        }
        return StringOut;
    }

public:

    PaintBitmap() : Width(0), Height(0), Pixels(std::vector<Paint::Pixel>()) { };
    PaintBitmap(const std::string& Base64)
    {
        std::string Dec = PaintBitmap::Base64Decode(Base64);
        memcpy(&this->Width, &(Dec[0]), 4);
        memcpy(&this->Height, &(Dec[4]), 4);
        this->Pixels.resize(this->Width * this->Height);
        memcpy(&(this->Pixels[0]), &(Dec[8]), this->Pixels.size() * 4);
    };

    std::uint32_t GetWidth() const { return Width; };
    std::uint32_t GetHeight() const { return Height; };
    std::vector<Paint::Pixel> GetPixels() const { return Pixels; };
    void Paint(const Point& P) const { Paint::DrawPixels(Pixels, Width, Height, P); };

    static std::string GenerateBase64FromFile(const std::string& Filepath)
    {
        static const Paint::Pixel TransparancyMask { 255, 255, 255, 255 };
        std::uint32_t W = 0;
        std::uint32_t H = 0;
        std::vector<Paint::Pixel> Pxls;

        std::fstream BMPFile(Filepath, std::ios::in | std::ios::binary);
        if (BMPFile.is_open())
        {
            BMPFile.seekg(0, std::ios::end);
            int Length = BMPFile.tellg();
            BMPFile.seekg(0, std::ios::beg);
            std::vector<std::uint8_t> Buff(Length);
            BMPFile.read(reinterpret_cast<char*>(Buff.data()), 54);
            if ((Buff[0] != 'B' && Buff[1] != 'M') || ((Buff[28] != 24) && (Buff[28] != 32)))
            {
                BMPFile.close();
                return std::string();
            }
            std::uint16_t BPP = Buff[28];
            W = Buff[18] + (Buff[19] << 8);
            H = Buff[22] + (Buff[23] << 8);
            std::uint32_t PixelsOffset = Buff[10] + (Buff[11] << 8);
            std::uint32_t size = ((W * BPP + 31) / 32) * 4 * H;
            BMPFile.seekg (PixelsOffset, std::ios::beg);
            BMPFile.read(reinterpret_cast<char*>(Buff.data()), size);
            BMPFile.close();
            std::uint8_t* BuffPos = Buff.data();
            Pxls.resize(W * H);
            for (std::uint16_t Y = 0; Y < H; ++Y)
            {
                for (std::uint16_t X = 0; X < W; ++X)
                {
                    auto I = ((H - 1 - Y) * W + X);
                    Pxls[I].Blue = *(BuffPos++);
                    Pxls[I].Green = *(BuffPos++);
                    Pxls[I].Red = *(BuffPos++);
                    if (BPP > 24)
                        (BuffPos++);
                    if ((Pxls[I].Blue == TransparancyMask.Blue) &&
                        (Pxls[I].Green == TransparancyMask.Green) &&
                        (Pxls[I].Red == TransparancyMask.Red))
                        Pxls[I].Alpha = 0;

                    else
                        Pxls[I].Alpha = 255;
                }
                if (BPP == 24)
                    BuffPos += (-W * 3) & 3;
            }
        } else
        {
            return std::string();
        }

        std::uint32_t Size = (Pxls.size() + 2) * 4;
        std::string Buf;
        Buf.resize(Size);
        memcpy(&Buf[0], &(W), 4);
        memcpy(&Buf[4], &(H), 4);
        memcpy(&Buf[8], &(Pxls[0]), Pxls.size() * 4);
        return PaintBitmap::Base64Encode(Buf);
    };

private:
    std::uint32_t Width;
    std::uint32_t Height;
    std::vector<Paint::Pixel> Pixels;
};

#endif // PAINTBITMAP_HPP_INCLUDED