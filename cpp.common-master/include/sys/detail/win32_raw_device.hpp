#pragma once

#include <windows.h>
#include <string>
#include <cstdint>
#include <memory>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
namespace {


}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
namespace sys::detail::win32 {

    using namespace std;

    class raw_device
    {
    public:
        typedef HANDLE handle_type;

    public:
        raw_device(int sector_size = 0x200)
            : m_handle{ INVALID_HANDLE_VALUE }, m_device_size{ 0 }
            , m_sector_size{ sector_size }
        {}

        auto open(string const& disk) -> bool
        {
            wstring wpath; wpath.assign(disk.begin(), disk.end());

            m_handle = CreateFile(wpath.c_str(),
                GENERIC_READ,
                FILE_SHARE_READ,
                0,
                OPEN_EXISTING,
                0,
                0);

            if (m_handle == INVALID_HANDLE_VALUE)
                return false;

            m_device_size = size();
            return true;
        }

        auto close() -> void
        {
            if (m_handle != INVALID_HANDLE_VALUE)
                CloseHandle(m_handle);
        }

        auto read(char* buffer, int count) -> int64_t
        {
            if (!is_open() || count < 0 || m_offset >= m_device_size)
                return -1;

            try
            {
                auto remained = m_offset % m_sector_size;
                if (remained != 0)
                    if (set_file_pointer(m_offset) < -1) return -1;

                auto ss = m_sector_size;
                auto to_read = uint32_t(remained + count + ss - 1) / ss * ss;

                unique_ptr<char[]> tmp_buffer(new char[to_read]);

                DWORD rd;
                auto r = ReadFile(m_handle, tmp_buffer.get(), to_read, &rd, 0);
                if (!r)
                    return -1;

                auto read_size = (std::min)(int(rd), count);
                memcpy(buffer, tmp_buffer.get() + remained, read_size);
                m_offset += read_size;

                return read_size;
            }
            catch (...)
            {
            }

            return -1;
        }

        auto seek(int64_t offset, int dir) -> int64_t
        {
            if (!is_open())
                return -1;

            switch (dir)
            {
            case 0: m_offset = offset;
                break;
            case 1: m_offset += offset;
                break;
            case 2: m_offset = m_device_size + offset;
                break;
            default:
                return -1;
            }

            return set_file_pointer(m_offset) < 0
                ? -1
                : m_offset;
        }

        auto size() const -> int64_t
        {
            if (!is_open())
                return -1;

            DWORD dw_read = 0;
            DISK_GEOMETRY pdg = { 0 };

            auto r = DeviceIoControl(m_handle,
                IOCTL_DISK_GET_DRIVE_GEOMETRY,
                NULL, 0,
                &pdg, sizeof(pdg),
                &dw_read,
                (LPOVERLAPPED)NULL);

            return r > 0
                ? int64_t(pdg.Cylinders.QuadPart) * pdg.TracksPerCylinder
                * pdg.SectorsPerTrack * pdg.BytesPerSector
                : -1;
        }

        auto is_open() const -> bool
        {
            return m_handle != INVALID_HANDLE_VALUE && m_device_size > 0;
        }


        auto tell() -> int64_t
        {
            return m_offset;
            /*
            auto hi = 0L;
            auto lo = 0L;
            auto move_pos = SetFilePointer(m_handle, lo, &hi, FILE_CURRENT);

            return (int64_t(hi) << 32) | move_pos;
            */
        }

        static auto size(string const& name) -> int64_t
        {
            raw_device rd;
            return rd.open(name) ? rd.size() : -1;
        }

        auto last_error() -> uint32_t
        {
            return static_cast<uint32_t>(GetLastError());
        }

    private:
        auto set_file_pointer(int64_t offset) -> int64_t
        {
            int64_t res = -1;

            try
            {
                auto remained = offset % 0x200;
                auto seek_offset = offset - remained;
                auto hi = LONG(seek_offset >> 32);
                auto lo = LONG(seek_offset & 0xffffffff);
                auto move_pos = SetFilePointer(m_handle, lo, &hi, FILE_BEGIN);

                res = (int64_t(hi) << 32) | move_pos;
            }
            catch (...)
            {
            }

            return res;
        }

    private:
        handle_type m_handle;

        int64_t m_offset;
        int     m_sector_size;
        int64_t m_device_size;
    };

}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
