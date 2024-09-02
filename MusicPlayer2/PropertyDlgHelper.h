﻿#pragma once
#include "SongInfo.h"

class CPropertyDlgHelper
{
public:
    CPropertyDlgHelper(const vector<SongInfo>& songs);
    ~CPropertyDlgHelper();

    wstring GetMultiFileName();
    wstring GetMultiFilePath();
    wstring GetMultiType();
    wstring GetMultiLength();
    wstring GetMultiSize();
    wstring GetMultiBitrate();
    wstring GetMultiChannels();
    wstring GetMultiFreq();
    wstring GetMultiBits();

    wstring GetMultiTitle();
    wstring GetMultiArtist();
    wstring GetMultiAlbum();
    wstring GetMultiTrack();
    wstring GetMultiYear();
    wstring GetMultiGenre();
    wstring GetMultiComment();

    bool IsMultiWritable();
    bool IsMultiCoverWritable();

    bool IsTitleModified(const vector<SongInfo>& list_ori);
    bool IsArtistModified(const vector<SongInfo>& list_ori);
    bool IsAlbumModified(const vector<SongInfo>& list_ori);
    bool IsTrackModified(const vector<SongInfo>& list_ori);
    bool IsYearModified(const vector<SongInfo>& list_ori);
    bool IsGenreModified(const vector<SongInfo>& list_ori);
    bool IsCommentModified(const vector<SongInfo>& list_ori);

private:
    //获取m_song_info中一个字段的值，如果该字段全部相同，则返回该值，如果有一个不同，则返回“多个数值”
    static wstring GetMultiValue(std::function<wstring(const SongInfo&)> fun_get_value, const vector<SongInfo>& song_list);

    //判断m_song_info中的一个字段和list_ori是否有不同，如果有则返回true
    bool IsValueModified(std::function<wstring(const SongInfo&)> fun_get_value, const vector<SongInfo>& list_ori);

private:
    const vector<SongInfo>& m_song_info;
};
