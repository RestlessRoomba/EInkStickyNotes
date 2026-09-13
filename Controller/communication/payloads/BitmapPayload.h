#pragma once

#include <cstdint>
#include <vector>


class BitmapPayload
{
    public:
        BitmapPayload(const std::vector<std::uint8_t>& bitmap);

        std::vector<std::uint8_t> serialize() const;
        static BitmapPayload deserialize(const std::vector<std::uint8_t>& data);

        const std::vector<std::uint8_t> bitmap() const;

    private:
        std::vector<std::uint8_t> m_bitmap;
};
