#ifndef __GafFeatures_SCENE_H__
#define __GafFeatures_SCENE_H__

#include "axmol.h"
#include "GAF.h"

#include <string>
#include <vector>
#include <unordered_map>

#include "GAFMacros.h"
#include "GAFSoundInfo.h"


NS_GAF_BEGIN
class GAFObject;
class GAFAsset;
NS_GAF_END

USING_NS_GAF;

//#define SEARCH_ALL_GAF_FILES

class GafFeatures : public ax::Layer
{
private:

    typedef std::vector<std::string> StringVector_t;
    typedef std::unordered_map<int, std::string> MusicEffects_t;

    GAFAsset*                m_asset;
    ax::Vector<GAFObject*>   m_objects;
    StringVector_t           m_files;
    int                      m_anim_index;
    MusicEffects_t           m_musicEffects;
    void enableSequenceControllers(bool value);

    typedef std::vector<std::string> ObjectSequencesNames_t;
    ObjectSequencesNames_t  m_objectSequencesNames;
    int m_currentSequence;

    ax::Label* m_loadingTimeLabel;
    ax::EventListenerTouchAllAtOnce* m_touchlistener;

    ax::MenuItemImage* m_playButton;
    ax::MenuItemImage* m_pauseButton;
    ax::MenuItemImage* m_nextSequence;
    ax::MenuItemImage* m_prevSequence;
    ax::Label*         m_sequenceName;
    ax::Label*         m_vramStat;

    void setupMenuItems();
    ax::MenuItemImage* addButton(const std::string& buttonName,
                                 const std::string& buttonPressedName,
                                 const ax::Point& pos,
                                 const ax::ccMenuCallback& clb);

    void generateGafFilesList();
#ifdef SEARCH_ALL_GAF_FILES
#if defined(WIN32)
    void searchGafFilesInDirectory(std::wstring& path);
#elif defined(__APPLE__)
    void searchGafFilesInDirectory(std::string& path);
#endif
#endif // SEARCH_ALL_GAF_FILES

public:
    GafFeatures();
    ~GafFeatures() override;

    bool init() override;

    // there's no 'id' in cpp, so we recommend to return the class instance pointer
    static ax::Scene* scene();

    // preprocessor macro for "static create()" constructor ( node() deprecated )
    CREATE_FUNC(GafFeatures);

    virtual void onTouchesBegan(const std::vector<ax::Touch*>& touches, ax::Event *unused_event);

    void black(ax::Object*);
    void white(ax::Object*);
    void gray(ax::Object*);
    void prevFrame(ax::Object*);
    void nextFrame(ax::Object*);
    void toggleReverse(ax::Object*);
    void restart(ax::Object*);
    void playpause(ax::Object*);
    void cleanup(ax::Object*);
    void next_anim(ax::Object*);
    void prev_anim(ax::Object*);
    int  maxFrameNumber();
    void setFrameNumber(int aFrameNumber);
    int  frameNumber();
    void addObjectsToScene();
    void removeFromScene(ssize_t aCount);

    void nextSequence(ax::Object*);
    void prevSequence(ax::Object*);

    virtual void onFinishSequence(GAFObject* object, const std::string& sequenceName);

    virtual void onFramePlayed(GAFObject* object, uint32_t frame);

    virtual void onSoundEvent(GAFSoundInfo* sound, int32_t repeat, GAFSoundInfo::SyncEvent syncEvent);
};

#endif // __GafFeatures_SCENE_H__
