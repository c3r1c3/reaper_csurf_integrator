//
//  control_surface_integrator.cpp
//  reaper_control_surface_integrator
//
//

#include "control_surface_integrator.h"
#include "control_surface_midi_widgets.h"
#include "control_surface_Reaper_actions.h"
#include "control_surface_manager_actions.h"

#include "WDL/lineparse.h"
#include "WDL/projectcontext.h"

const string ChannelLeft = "ChannelLeft";
const string ChannelRight = "ChannelRight";
const string BankLeft = "BankLeft";
const string BankRight = "BankRight";

const string Fader = "Fader";
const string Rotary = "Rotary";
const string RotaryPush = "RotaryPush";
const string Display = "Display";
const string FaderTouch = "FaderTouched";
const string Solo = "Solo";
const string RecordArm = "RecordArm";
const string Select = "Select";
const string Mute = "Mute";

const string NextMap = "NextMap";
const string LockTracks = "LockTracks";
const string UnlockTracks = "UnlockTracks";

const string Read = "Read";
const string Write = "Write";
const string Trim = "Trim";
const string Touch = "Touch";
const string Latch = "Latch";
const string Group = "Group";

const string Save = "Save";
const string Undo = "Undo";
const string Cancel = "Cancel";
const string Enter = "Enter";

const string Marker = "Marker";
const string Nudge = "Nudge";
const string Cycle = "Cycle";
const string Click = "Click";

const string Rewind = "Rewind";
const string FastForward = "FastForward";
const string Stop = "Stop";
const string Play = "Play";
const string Record = "Record";

const string Up = "Up";
const string Down = "Down";
const string Left = "Left";
const string Right = "Right";
const string Zoom = "Zoom";
const string Scrub = "Scrub";

const string ChannelInputMeterLeft = "TrackInMeterLeft";
const string ChannelInputMeterRight = "TrackInMeterRight";
const string CompressorMeter = "CompressorMeter";
const string GateMeter = "GateMeter";
const string ChannelOutputMeterLeft = "ChannelOutputMeterLeft";
const string ChannelOutputMeterRight = "ChannelOutputMeterRight";

const string DisplayFX = "DisplayFX";
const string SendsMode = "SendsMode";

const string  Equalizer = "Equalizer";

const string  LoCurve = "LoCurve";
const string  HiCurve = "HiCurve";
const string  HiGain = "HiGain";
const string  HiFrequency = "HiFrequency";
const string  HiMidGain = "HiMidGain";
const string  HiMidFrequency = "HiMidFrequency";
const string  HiMidQ = "HiMidQ";
const string  LoMidGain = "LoMidGain";
const string  LoMidFrequency = "LoMidFrequency";
const string  LoMidQ = "LoMidQ";
const string  LoGain = "LoGain";
const string  LoFrequency = "LoFrequency";

const string Compressor = "Compressor";

const string Threshold = "Threshold";
const string Release = "Release";
const string Ratio = "Ratio";
const string Parallel = "Parallel";
const string Attack = "Attack";

const string Drive = "Drive";
const string Character = "Character";

////////////////////////////////////////////////////////////////////////////////////////////////////////
// MidiWidget
////////////////////////////////////////////////////////////////////////////////////////////////////////
void MidiWidget::AddToRealSurface(RealSurface* surface)
{
    surface->AddWidgetToNameMap(GetName(), this);
}

void MidiWidget::Update()
{
    // this is the turnaround point, now we head back up the chain eventually leading to Action ->
    GetRealSurface()->UpdateAction(GetGUID(), GetActionName(), GetName());
}

void MidiWidget::ForceUpdate()
{
    // this is the turnaround point, now we head back up the chain eventually leading to Action ->
    GetRealSurface()->ForceUpdateAction(GetGUID(), GetActionName(), GetName());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RealSurfaceChannel
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RealSurfaceChannel::SetGUID(string GUID)
{
    GUID_ = GUID;
    
    for (auto widgetName : widgetNames_)
        realSurface_->SetWidgetGUID(widgetName, GUID);
    
    realSurface_->GetSurfaceGroup()->GetLogicalSurface()->MapTrack(GUID_);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// RealSurface
////////////////////////////////////////////////////////////////////////////////////////////////////////

#define IDC_REPEAT                      1068
#define ID_FILE_SAVEAS                  40022
#define ID_FILE_NEWPROJECT              40023
#define ID_FILE_OPENPROJECT             40025
#define ID_FILE_SAVEPROJECT             40026
#define IDC_EDIT_UNDO                   40029
#define IDC_EDIT_REDO                   40030
#define ID_MARKER_PREV                  40172
#define ID_MARKER_NEXT                  40173
#define ID_INSERT_MARKERRGN             40174
#define ID_INSERT_MARKER                40157
#define ID_LOOP_SETSTART                40222
#define ID_LOOP_SETEND                  40223
#define ID_METRONOME                    40364
#define ID_GOTO_MARKER1                 40161
#define ID_SET_MARKER1                  40657

void RealSurface::MapRealSurfaceActions()
{
    // GAW TBD -- this will be in .axm files
    
    LogicalSurface* logicalSurface = GetSurfaceGroup()->GetLogicalSurface();
    string actionBaseAddress = RealControlSurface + GetSurfaceGroup()->GetName() + GetName();;
    
    AddAction(actionBaseAddress + ChannelLeft, new TrackBank_Action(logicalSurface, -1));
    AddAction(actionBaseAddress + ChannelRight, new TrackBank_Action(logicalSurface, 1));
    AddAction(actionBaseAddress + BankLeft, new TrackBank_Action(logicalSurface, -8));
    AddAction(actionBaseAddress + BankRight, new TrackBank_Action(logicalSurface, 8));
    
    AddAction(actionBaseAddress + NextMap, new NextMap_Action(logicalSurface));
    AddAction(actionBaseAddress + LockTracks, new ImmobilizeSelectedTracks_Action(logicalSurface));
    AddAction(actionBaseAddress + UnlockTracks, new MobilizeSelectedTracks_Action(logicalSurface));
    
    AddAction(actionBaseAddress + Shift, new Shift_Action(logicalSurface));
    AddAction(actionBaseAddress + Option, new Option_Action(logicalSurface));
    AddAction(actionBaseAddress + Control, new Control_Action(logicalSurface));
    AddAction(actionBaseAddress + Alt, new Alt_Action(logicalSurface));
    
    AddAction(actionBaseAddress + Read, new TrackAutoMode_Action(logicalSurface, 1));
    AddAction(actionBaseAddress + Write, new TrackAutoMode_Action(logicalSurface, 3));
    AddAction(actionBaseAddress + Trim, new TrackAutoMode_Action(logicalSurface, 0));
    AddAction(actionBaseAddress + Touch, new TrackAutoMode_Action(logicalSurface, 2));
    AddAction(actionBaseAddress + Latch, new TrackAutoMode_Action(logicalSurface, 4));
    AddAction(actionBaseAddress + Group, new TrackAutoMode_Action(logicalSurface, 5));
    
    AddAction(actionBaseAddress + Shift + Read, new GlobalAutoMode_Action(logicalSurface, 1));
    AddAction(actionBaseAddress + Shift + Write, new GlobalAutoMode_Action(logicalSurface, 3));
    AddAction(actionBaseAddress + Shift + Trim, new GlobalAutoMode_Action(logicalSurface, 0));
    AddAction(actionBaseAddress + Shift + Touch, new GlobalAutoMode_Action(logicalSurface, 2));
    AddAction(actionBaseAddress + Shift + Latch, new GlobalAutoMode_Action(logicalSurface, 4));
    AddAction(actionBaseAddress + Shift + Group, new GlobalAutoMode_Action(logicalSurface, 5));
    
    AddAction(actionBaseAddress + Save, new Reaper_Action(logicalSurface, ID_FILE_SAVEPROJECT));
    AddAction(actionBaseAddress + Shift + Save, new Reaper_Action(logicalSurface, ID_FILE_SAVEAS));
    AddAction(actionBaseAddress + Undo, new Reaper_Action(logicalSurface, IDC_EDIT_UNDO));
    AddAction(actionBaseAddress + Shift + Undo, new Reaper_Action(logicalSurface, IDC_EDIT_REDO));
    
    //logicalSurfaceInteractor_->AddAction(new Enter_Action(Enter, logicalSurfaceInteractor_));
    //logicalSurfaceInteractor_->AddAction(new Cancel_Action(Cancel, logicalSurfaceInteractor_));
    
    AddAction(actionBaseAddress + Marker, new Reaper_Action(logicalSurface, ID_MARKER_PREV));
    AddAction(actionBaseAddress + Shift + Marker, new Reaper_Action(logicalSurface, ID_INSERT_MARKER));
    AddAction(actionBaseAddress + Option + Marker, new Reaper_Action(logicalSurface,ID_INSERT_MARKERRGN));
    AddAction(actionBaseAddress + Nudge, new Reaper_Action(logicalSurface, ID_MARKER_NEXT));
    AddAction(actionBaseAddress + Cycle, new CycleTimeline_Action(logicalSurface));
    AddAction(actionBaseAddress + Click, new Reaper_Action(logicalSurface, ID_METRONOME));
    
    AddAction(actionBaseAddress + Rewind, new Rewind_Action(logicalSurface));
    AddAction(actionBaseAddress + FastForward, new FastForward_Action(logicalSurface));
    AddAction(actionBaseAddress + Stop, new Stop_Action(logicalSurface));
    AddAction(actionBaseAddress + Play, new Play_Action(logicalSurface));
    AddAction(actionBaseAddress + Record, new Record_Action(logicalSurface));
    
    AddAction(actionBaseAddress + Up, new RepeatingArrow_Action(logicalSurface, 0, 0.3));
    AddAction(actionBaseAddress + Down, new RepeatingArrow_Action(logicalSurface, 1, 0.3));
    AddAction(actionBaseAddress + Left, new RepeatingArrow_Action(logicalSurface, 2, 0.3));
    AddAction(actionBaseAddress + Right, new RepeatingArrow_Action(logicalSurface, 3, 0.3));
    
    AddAction(actionBaseAddress + Zoom, new LatchedZoom_Action(logicalSurface));
    AddAction(actionBaseAddress + Scrub, new LatchedScrub_Action(logicalSurface));
    
    AddAction(actionBaseAddress + DisplayFX, new SetShowFXWindows_Action(logicalSurface));
}

void RealSurface::InitFXMaps()
{
    // GAW TBD -- this will be in .fxt files

    FXMap* fxMap = new FXMap("VST: ReaComp (Cockos)");

    fxMap->AddEntry(Threshold, "Thresh");
    fxMap->AddEntry(Character, "Gain");
    fxMap->AddEntry(Attack, "Attack");
    fxMap->AddEntry(Release, "Release");
    fxMap->AddEntry(Ratio, "Ratio");
    fxMap->AddEntry(Compressor, "Bypass");
    fxMap->AddEntry(Parallel, "Wet");
    fxMap->AddEntry(CompressorMeter, GainReduction_dB);
    
    AddFXMap(fxMap);
    
    fxMap = new FXMap("VST: UAD Fairchild 660 (Universal Audio, Inc.)");

    fxMap->AddEntry(Threshold, "Thresh");
    fxMap->AddEntry(Character, "Output");
    fxMap->AddEntry(Drive, "Meter");
    fxMap->AddEntry(Attack, "Headroom");
    fxMap->AddEntry(Release, "Input");
    fxMap->AddEntry(Ratio, "Time Const");
    fxMap->AddEntry(Compressor, "Bypass");
    fxMap->AddEntry(Parallel, "Wet");
    
    AddFXMap(fxMap);
    
    fxMap = new FXMap("VST: UAD Teletronix LA-2A Silver (Universal Audio, Inc.)");
    
    fxMap->AddEntry(Threshold, "Peak Reduct");
    fxMap->AddEntry(Character, "Gain");
    fxMap->AddEntry(Drive, "Meter");
    fxMap->AddEntry(Attack, "Emphasis");
    fxMap->AddEntry(Ratio, "Comp/Limit");
    fxMap->AddEntry(Compressor, "Bypass");
    fxMap->AddEntry(Parallel, "Wet");
    
    AddFXMap(fxMap);
    
    fxMap = new FXMap("VST: UAD Harrison 32C (Universal Audio, Inc.)");
    
    fxMap->AddEntry(LoCurve, "LowPeak");
    //fxMap->AddEntry(HiCurve, "");
    fxMap->AddEntry(HiGain, "HiGain");
    fxMap->AddEntry(HiFrequency, "HiFreq");
    fxMap->AddEntry(HiMidGain, "HiMidGain");
    fxMap->AddEntry(HiMidFrequency, "HiMidFreq");
    fxMap->AddEntry(HiMidQ, "LowPass");
    fxMap->AddEntry(LoMidGain, "LoMidGain");
    fxMap->AddEntry(LoMidFrequency, "LoMidFreq");
    fxMap->AddEntry(LoMidQ, "HiPass");
    fxMap->AddEntry(LoGain, "LowGain");
    fxMap->AddEntry(LoFrequency, "LowFreq");
    fxMap->AddEntry(Equalizer, "Bypass");
    
    AddFXMap(fxMap);
    
    fxMap = new FXMap("VST: UAD Pultec EQP-1A (Universal Audio, Inc.)");
    
    //fxMap->AddEntry(LoCurve, "");
    //fxMap->AddEntry(HiCurve, "");
    fxMap->AddEntry(HiGain, "HF Atten");
    fxMap->AddEntry(HiFrequency, "HF Atten Freq");
    fxMap->AddEntry(HiMidGain, "HF Boost");
    fxMap->AddEntry(HiMidFrequency, "High Freq");
    fxMap->AddEntry(HiMidQ, "HF Q");
    fxMap->AddEntry(LoMidGain, "LF Atten");
    fxMap->AddEntry(LoMidFrequency, "Low Freq");
    //fxMap->AddEntry(LoMidQ, "");
    fxMap->AddEntry(LoGain, "LF Boost");
    fxMap->AddEntry(LoFrequency, "Low Freq");
    fxMap->AddEntry(Equalizer, "Bypass");
    
    AddFXMap(fxMap);
    
    fxMap = new FXMap("VST: UAD Pultec MEQ-5 (Universal Audio, Inc.)");
    
    //fxMap->AddEntry(LoCurve, "");
    //fxMap->AddEntry(HiCurve, "");
    fxMap->AddEntry(HiGain, "HM Peak");
    fxMap->AddEntry(HiFrequency, "HM Freq");
    fxMap->AddEntry(HiMidGain, "Mid Dip");
    fxMap->AddEntry(HiMidFrequency, "Mid Freq");
    //fxMap->AddEntry(HiMidQ, "");
    fxMap->AddEntry(LoMidGain, "LM Peak");
    fxMap->AddEntry(LoMidFrequency, "LM Freq");
    //fxMap->AddEntry(LoMidQ, "");
    //fxMap->AddEntry(LoGain, "");
    //fxMap->AddEntry(LoFrequency, "");
    fxMap->AddEntry(Equalizer, "Bypass");
    
    AddFXMap(fxMap);
}

void RealSurface::MapTrackToWidgets(MediaTrack *track)
{
    string trackGUID = DAW::GetTrackGUIDAsString(DAW::CSurf_TrackToID(track, false));
    
    for(auto* channel : channels_)
        channel->SetGUID(trackGUID);
}

void  RealSurface::UnmapWidgetsFromTrack(MediaTrack *track)
{
    for(auto* channel : channels_)
        channel->SetGUID("");
}

void RealSurface::UnmapFXFromWidgets(MediaTrack *track)
{
    for(auto [widgetName, GUID] : remappedFXWidgets_)
        if(widgetsByName_.count(widgetName) > 0)
            widgetsByName_[widgetName]->SetGUID(GUID);
    
    remappedFXWidgets_.clear();
}

void RealSurface::MapFXToWidgets(MediaTrack *track)
{
    char fxName[BUFSZ];
    char fxGUID[BUFSZ];
    char fxParamName[BUFSZ];

    DeleteFXWindows();
    UnmapFXFromWidgets(track);
    
    string trackGUID = DAW::GetTrackGUIDAsString(DAW::CSurf_TrackToID(track, false));

    for(int i = 0; i < DAW::TrackFX_GetCount(track); i++)
    {
        DAW::TrackFX_GetFXName(track, i, fxName, sizeof(fxName));
        DAW::guidToString(DAW::TrackFX_GetFXGUID(track, i), fxGUID);
        
        if(fxMaps_.count(fxName) > 0)
        {
            FXMap* map = fxMaps_[fxName];
            
            for(auto mapEntry : map->GetMapEntries())
            {
                if(mapEntry.paramName == GainReduction_dB)
                    SetWidgetFXGUID(mapEntry.widgetName, trackGUID + fxGUID);
                else
                    for(int j = 0; j < DAW::TrackFX_GetNumParams(track, i); DAW::TrackFX_GetParamName(track, i, j++, fxParamName, sizeof(fxParamName)))
                        if(mapEntry.paramName == fxParamName)
                            SetWidgetFXGUID(mapEntry.widgetName, trackGUID + fxGUID);
            }
            
            AddFXWindow(FXWindow(track, fxGUID));
        }
    }
    
    OpenFXWindows();
    
    ForceUpdateWidgets();
}

void RealSurface::MapTrackAndFXActions(string trackGUID)
{
    LogicalSurface* logicalSurface = GetSurfaceGroup()->GetLogicalSurface();
    MediaTrack* track = DAW::GetTrackFromGUID(trackGUID);
    string actionBaseAddress = trackGUID + GetSurfaceGroup()->GetName() + GetName();
    
    // GAW TBD -- this will be in .axm files

    if(GetName() == "Console1")
    {
        AddAction(actionBaseAddress + Fader, new TrackVolume_Action(logicalSurface, track));
        AddAction(actionBaseAddress + Rotary, new TrackPan_Action(logicalSurface, track, 0x00));
        AddAction(actionBaseAddress + Mute, new TrackMute_Action(logicalSurface, track));
        AddAction(actionBaseAddress + Solo, new TrackSolo_Action(logicalSurface, track));

        AddAction(actionBaseAddress + ChannelInputMeterLeft, new TrackOutputMeter_Action(logicalSurface, track, 0));
        AddAction(actionBaseAddress + ChannelInputMeterRight, new TrackOutputMeter_Action(logicalSurface, track, 1));
        
        AddAction(actionBaseAddress + ChannelOutputMeterLeft, new TrackOutputMeter_Action(logicalSurface, track, 0));
        AddAction(actionBaseAddress + ChannelOutputMeterRight, new TrackOutputMeter_Action(logicalSurface, track, 1));
        
        AddAction(actionBaseAddress + TrackOnSelection, new MapTrackToWidgets_Action(logicalSurface, track));
        AddAction(actionBaseAddress + TrackOnSelection, new MapFXToWidgets_Action(logicalSurface, track));
    }
    else
    {
        AddAction(actionBaseAddress + Display, new TrackNameDisplay_Action(logicalSurface, track));
        AddAction(actionBaseAddress + Display, new TrackTouchControlled_Action(logicalSurface, track, new TrackVolumeDisplay_Action(logicalSurface, track), actionBaseAddress + Display));
        
        AddAction(actionBaseAddress + Fader, new TrackVolume_Action(logicalSurface, track));
        AddAction(actionBaseAddress + FaderTouch, new TrackTouch_Action(logicalSurface, track));
        
        CycledAction* cycledAction = new CycledAction(logicalSurface);
        cycledAction->AddAction(new TrackPan_Action(logicalSurface, track, 0x00));
        cycledAction->AddAction(new TrackPanWidth_Action(logicalSurface, track, 0x30));
        AddAction(actionBaseAddress + Rotary, cycledAction);
        AddAction(actionBaseAddress + RotaryPush, cycledAction);
        
        AddAction(actionBaseAddress + Select, new TrackUniqueSelect_Action(logicalSurface, track));
        AddAction(actionBaseAddress + Shift + Select, new TrackRangeSelect_Action(logicalSurface, track));
        AddAction(actionBaseAddress + Control + Select, new TrackSelect_Action(logicalSurface, track));
        
        AddAction(actionBaseAddress + RecordArm, new TrackRecordArm_Action(logicalSurface, track));
        AddAction(actionBaseAddress + Mute, new TrackMute_Action(logicalSurface, track));
        AddAction(actionBaseAddress + Solo, new TrackSolo_Action(logicalSurface, track));
    }
        

    MapFXActions(track);
}

void RealSurface::MapFXActions(MediaTrack* track)
{
    LogicalSurface* logicalSurface = GetSurfaceGroup()->GetLogicalSurface();
    
    char fxName[BUFSZ];
    char fxParamName[BUFSZ];
    char fxGUID[BUFSZ];
    
    string trackGUID = DAW::GetTrackGUIDAsString(DAW::CSurf_TrackToID(track, false));
    
    for(int i = 0; i < DAW::TrackFX_GetCount(track); i++)
    {
        DAW::TrackFX_GetFXName(track, i, fxName, sizeof(fxName));
        
        if(fxMaps_.count(fxName) > 0)
        {
            FXMap* map = fxMaps_[fxName];
            DAW::guidToString(DAW::TrackFX_GetFXGUID(track, i), fxGUID);
            string actionTrackBaseAddress = trackGUID + GetSurfaceGroup()->GetName() + GetName();
            string actionBaseAddress = trackGUID + fxGUID + GetSurfaceGroup()->GetName() + GetName();

            for(auto mapEntry : map->GetMapEntries())
            {
                if(mapEntry.paramName == GainReduction_dB)
                    AddAction(actionBaseAddress + mapEntry.widgetName, new TrackGainReductionMeter_Action(logicalSurface, track, fxGUID));
                else
                {
                    for(int j = 0; j < DAW::TrackFX_GetNumParams(track, i); j++)
                    {
                        DAW::TrackFX_GetParamName(track, i, j, fxParamName, sizeof(fxParamName));
                        
                        if(mapEntry.paramName == fxParamName)
                            AddAction(actionBaseAddress + mapEntry.widgetName, new TrackFX_Action(logicalSurface, track, fxGUID, j));
                    }
                }
            }
        }
        
        if(GetSurfaceGroup()->GetLogicalSurface()->GetManager()->GetVSTMonitor() && GetSurfaceGroup()->GetLogicalSurface()->GetManager()->GetIsInitialized())
        {
            DAW::ShowConsoleMsg(("\n\n" + string(fxName) + "\n").c_str());
            
            for(int j = 0; j < DAW::TrackFX_GetNumParams(track, i); j++)
            {
                DAW::TrackFX_GetParamName(track, i, j, fxParamName, sizeof(fxParamName));
                DAW::ShowConsoleMsg((string(fxParamName) + "\n").c_str());
            }
        }
    }
}

void RealSurface::AddAction(string actionAddress, Action* action)
{
    GetSurfaceGroup()->GetLogicalSurface()->AddAction(actionAddress, action);
}

// to Actions ->
double RealSurface::GetActionCurrentNormalizedValue(string GUID, string actionName, string widgetName)
{
    return GetSurfaceGroup()->GetActionCurrentNormalizedValue(GUID, GetName(), actionName, widgetName);
}

void RealSurface::UpdateAction(string GUID, string actionName, string widgetName)
{
    GetSurfaceGroup()->UpdateAction(GUID, GetName(), actionName, widgetName);
}

void RealSurface::ForceUpdateAction(string GUID, string actionName, string widgetName)
{
    GetSurfaceGroup()->ForceUpdateAction(GUID, GetName(), actionName, widgetName);
}

void RealSurface::CycleAction(string GUID, string actionName, string widgetName)
{
    GetSurfaceGroup()->CycleAction(GUID, GetName(), actionName, widgetName);
}

void RealSurface::DoAction(string GUID, string actionName, string widgetName, double value)
{
    GetSurfaceGroup()->DoAction(value, GUID, GetName(), actionName, widgetName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// SurfaceGroup
////////////////////////////////////////////////////////////////////////////////////////////////////////
// to Actions ->
double SurfaceGroup::GetActionCurrentNormalizedValue(string GUID, string surfaceName, string actionName, string widgetName)
{
    return GetLogicalSurface()->GetActionCurrentNormalizedValue(ActionAddressFor(GUID, surfaceName, actionName), GetName(), surfaceName, widgetName);
}

void SurfaceGroup::UpdateAction(string GUID, string surfaceName, string actionName, string widgetName)
{
    GetLogicalSurface()->UpdateAction(ActionAddressFor(GUID, surfaceName, actionName), GetName(), surfaceName, widgetName);
}

void SurfaceGroup::ForceUpdateAction(string GUID, string surfaceName, string actionName, string widgetName)
{
    GetLogicalSurface()->ForceUpdateAction(ActionAddressFor(GUID, surfaceName, actionName), GetName(), surfaceName, widgetName);
}

void SurfaceGroup::CycleAction(string GUID, string surfaceName, string actionName, string widgetName)
{
    GetLogicalSurface()->CycleAction(ActionAddressFor(GUID, surfaceName, actionName), GetName(), surfaceName, widgetName);
}

void SurfaceGroup::DoAction(double value, string GUID, string surfaceName, string actionName, string widgetName)
{
    GetLogicalSurface()->DoAction(ActionAddressFor(GUID, surfaceName, actionName), value, GetName(), surfaceName, widgetName);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
// CSurfManager
////////////////////////////////////////////////////////////////////////////////////////////////////////




/*
 void SID_PCM_Source::SaveState(ProjectStateContext * ctx)
 {
 ctx->AddLine("FILE \"%s\" %f %d %d %d %d", m_sidfn.toRawUTF8(),m_sidlen,m_sid_track, m_sid_channelmode,m_sid_sr,m_sid_render_mode);
 }
 
 int SID_PCM_Source::LoadState(const char * firstline, ProjectStateContext * ctx)
 {
 LineParser lp;
 char buf[2048];
 for (;;)
 {
 if (ctx->GetLine(buf, sizeof(buf)))
 break;
 lp.parse(buf);
 if (strcmp(lp.gettoken_str(0), "FILE") == 0)
 {
 m_sidfn = String(CharPointer_UTF8(lp.gettoken_str(1)));
 m_sidlen = lp.gettoken_float(2);
 m_sid_track = lp.gettoken_int(3);
 m_sid_channelmode = lp.gettoken_int(4);
 m_sid_sr = lp.gettoken_int(5);
 m_sid_render_mode = lp.gettoken_int(6);
 }
 if (lp.gettoken_str(0)[0] == '>')
 {
 renderSID();
 return 0;
 }
 }
 return -1;
 }
 */



// C++17 version ////////////////////////////////////////////////////


void SaveState(ProjectStateContext * ctx)
{
    //ctx->AddLine("FILE \"%s\" %f %d %d %d %d", m_sidfn.toRawUTF8(),m_sidlen,m_sid_track, m_sid_channelmode,m_sid_sr,m_sid_render_mode);
}

template<typename T>
inline void setFromToken(LineParser& lp, int index, T& value)
{
    if constexpr (is_same<int, T>::value)
        value = lp.gettoken_int(index);
    else if constexpr (is_same<double, T>::value)
        value = lp.gettoken_float(index);
    else if constexpr (is_same<float, T>::value)
        value = static_cast<float>(lp.gettoken_float(index));
    else if constexpr (is_same<string, T>::value)
        value = string(lp.gettoken_str(index));
    //else
    //static_assert(false, "Type not supported by LineParser");
}

template<typename... Args>
inline void setFromTokens(LineParser& lp, Args&&... args)
{
    int index = 0;
    (setFromToken(lp, ++index, args), ...);
}

int LoadState(const char * firstline, ProjectStateContext * ctx)
{
    LineParser lp;
    char buf[2048];
    for (;;)
    {
        if (ctx->GetLine(buf, sizeof(buf)))
            break;
        lp.parse(buf);
        if (strcmp(lp.gettoken_str(0), "FILE") == 0)
        {
            //setFromTokens(lp, m_sidfn, m_sidlen, m_sid_track, m_sid_channelmode, m_sid_sr, m_sid_render_mode);
        }
        if (lp.gettoken_str(0)[0] == '>')
        {
            //renderSID();
            return 0;
        }
    }
    return -1;
}


void CSurfManager::InitRealSurfaces()
{
    // GAW TBD -- this will be in .rsm files

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // GAW TBD Hack an ini file so that testers can config MIDI IO for their local surfaces
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    const char *ptr = DAW::GetResourcePath();
    char localBuf[4096];
    
    strcpy( localBuf, ptr );
    
    strcpy( localBuf + strlen(localBuf), "/CSI/CSI.ini" );
    
    FILE *filePtr;
    
    filePtr = fopen(localBuf, "r" );
    
    if(! filePtr)
    {
        char errorBuf[4096];
        strcpy(errorBuf, "Can't locate ");
        strcpy(&errorBuf[13], localBuf);
        DAW::ShowConsoleMsg(errorBuf);
        return;
    }
    
    while (fgets(localBuf, sizeof(localBuf), filePtr))
        if(localBuf[0] == '/')
            continue;
        else
            break;
    
    char *p;
    char optionName[512];
    char optionValue[512];
    
    
    
    
    // Midi monitoring //////////////////////////
    bool midiInMonitor = false;
    
    p = strtok (localBuf,"=");
    
    strcpy( optionName, p );
    
    if(!strcmp(optionName, "MidiInMonitor"))
        if(p != NULL)
        {
            p = strtok(NULL, "=");
            
            if(p != NULL)
            {
                strcpy( optionValue, p );
                if(!strcmp(optionValue, "On\n"))
                    midiInMonitor = true;
                
            }
        }
    
    fgets(localBuf, sizeof(localBuf), filePtr);
    
    bool midiOutMonitor = false;
    
    p = strtok (localBuf,"=");
    
    strcpy( optionName, p );
    
    if(!strcmp(optionName, "MidiOutMonitor"))
        if(p != NULL)
        {
            p = strtok(NULL, "=");
            
            if(p != NULL)
            {
                strcpy( optionValue, p );
                if(!strcmp(optionValue, "On\n"))
                    midiOutMonitor = true;
                
            }
        }
    
    // VST monitoring //////////////////////////
    fgets(localBuf, sizeof(localBuf), filePtr);
    
    bool VSTMonitor = false;
    
    p = strtok (localBuf,"=");
    
    strcpy( optionName, p );
    
    if(!strcmp(optionName, "VSTMonitor"))
        if(p != NULL)
        {
            p = strtok(NULL, "=");
            
            if(p != NULL)
            {
                strcpy( optionValue, p );
                if(!strcmp(optionValue, "On\n"))
                    VSTMonitor = true;
                
            }
        }
    
    while (fgets(localBuf, sizeof(localBuf), filePtr))
    {
        if(localBuf[0] == '/')
            continue;
        
        int index = 0;
        char name[512];
        char numBankableChannelsString[512];
        char channelInString[512];
        char channelOutString[512];
        
        p = strtok (localBuf," ");
        
        strcpy( name, p );
        index++;
        
        while (p != NULL)
        {
            p = strtok(NULL, " ");
            
            if( p != NULL && index == 1)
            {
                strcpy(numBankableChannelsString, p);
                index++;
            }
            else if(p != NULL && index == 2)
            {
                strcpy(channelInString, p);
                index++;
            }
            else if(p != NULL && index == 3)
            {
                strcpy(channelOutString, p);
                index++;
            }
        }
        
        int numBankableChannels = atoi(numBankableChannelsString);
        
        int channelIn = atoi(channelInString);
        channelIn--; // MIDI channels are 0  based
        
        int channelOut = atoi(channelOutString);
        channelOut--; // MIDI channels are 0  based
        
        AddRealSurface(new MidiCSurf(name, numBankableChannels, GetMidiIOManager()->GetMidiInputForChannel(channelIn), GetMidiIOManager()->GetMidiOutputForChannel(channelOut), midiInMonitor, midiOutMonitor));
    }
    
    VSTMonitor_ = VSTMonitor;
    
    fclose ( filePtr );
}

void CSurfManager::InitRealSurface(RealSurface* surface)
{
    
    // GAW TBD -- this will be in .rst files
    
    RealSurfaceChannel* channel = nullptr;
    
    if(surface->GetName() == "Console1")
    {
        surface->AddWidget(new PushButtonWithLatch_MidiWidget(RealControlSurface, surface, DisplayFX, new MIDI_event_ex_t(0xb0, 0x66, 0x7f), new MIDI_event_ex_t(0xb0, 0x66, 0x00)));

        /*
        surface->AddWidget(new PushButton_CSurfWidget("PagePlus", surface,   new MIDI_event_ex_t(0xb0, 0x60, 0x7f), new MIDI_event_ex_t(0xb0, 0x60, 0x00)));
        surface->AddWidget(new PushButton_CSurfWidget("PageMinus", surface,  new MIDI_event_ex_t(0xb0, 0x61, 0x7f), new MIDI_event_ex_t(0xb0, 0x61, 0x00)));
        
        surface->AddWidget(new PushButton_CSurfWidget("Select1", surface,    new MIDI_event_ex_t(0xb0, 0x15, 0x7f), new MIDI_event_ex_t(0xb0, 0x15, 0x00)));
        surface->AddWidget(new PushButton_CSurfWidget("Select2", surface,    new MIDI_event_ex_t(0xb0, 0x16, 0x7f), new MIDI_event_ex_t(0xb0, 0x16, 0x00)));
        surface->AddWidget(new PushButton_CSurfWidget("Select3", surface,    new MIDI_event_ex_t(0xb0, 0x17, 0x7f), new MIDI_event_ex_t(0xb0, 0x17, 0x00)));
        surface->AddWidget(new PushButton_CSurfWidget("Select4", surface,    new MIDI_event_ex_t(0xb0, 0x18, 0x7f), new MIDI_event_ex_t(0xb0, 0x18, 0x00)));
        surface->AddWidget(new PushButton_CSurfWidget("Select5", surface,    new MIDI_event_ex_t(0xb0, 0x19, 0x7f), new MIDI_event_ex_t(0xb0, 0x19, 0x00)));
        surface->AddWidget(new PushButton_CSurfWidget("Select6", surface,    new MIDI_event_ex_t(0xb0, 0x1a, 0x7f), new MIDI_event_ex_t(0xb0, 0x1a, 0x00)));
        surface->AddWidget(new PushButton_CSurfWidget("Select7", surface,    new MIDI_event_ex_t(0xb0, 0x1b, 0x7f), new MIDI_event_ex_t(0xb0, 0x1b, 0x00)));
        surface->AddWidget(new PushButton_CSurfWidget("Select8", surface,    new MIDI_event_ex_t(0xb0, 0x1c, 0x7f), new MIDI_event_ex_t(0xb0, 0x1c, 0x00)));
        surface->AddWidget(new PushButton_CSurfWidget("Select9", surface,    new MIDI_event_ex_t(0xb0, 0x1d, 0x7f), new MIDI_event_ex_t(0xb0, 0x1d, 0x00)));
        surface->AddWidget(new PushButton_CSurfWidget("Select10", surface,   new MIDI_event_ex_t(0xb0, 0x1e, 0x7f), new MIDI_event_ex_t(0xb0, 0x1e, 0x00)));
        surface->AddWidget(new PushButton_CSurfWidget("Select11", surface,   new MIDI_event_ex_t(0xb0, 0x1f, 0x7f), new MIDI_event_ex_t(0xb0, 0x1f, 0x00)));
        surface->AddWidget(new PushButton_CSurfWidget("Select12", surface,   new MIDI_event_ex_t(0xb0, 0x20, 0x7f), new MIDI_event_ex_t(0xb0, 0x20, 0x00)));
        surface->AddWidget(new PushButton_CSurfWidget("Select13", surface,   new MIDI_event_ex_t(0xb0, 0x21, 0x7f), new MIDI_event_ex_t(0xb0, 0x21, 0x00)));
        surface->AddWidget(new PushButton_CSurfWidget("Select14", surface,   new MIDI_event_ex_t(0xb0, 0x22, 0x7f), new MIDI_event_ex_t(0xb0, 0x22, 0x00)));
        surface->AddWidget(new PushButton_CSurfWidget("Select15", surface,   new MIDI_event_ex_t(0xb0, 0x23, 0x7f), new MIDI_event_ex_t(0xb0, 0x23, 0x00)));
        surface->AddWidget(new PushButton_CSurfWidget("Select16", surface,   new MIDI_event_ex_t(0xb0, 0x24, 0x7f), new MIDI_event_ex_t(0xb0, 0x24, 0x00)));
        surface->AddWidget(new PushButton_CSurfWidget("Select17", surface,   new MIDI_event_ex_t(0xb0, 0x25, 0x7f), new MIDI_event_ex_t(0xb0, 0x25, 0x00)));
        surface->AddWidget(new PushButton_CSurfWidget("Select18", surface,   new MIDI_event_ex_t(0xb0, 0x26, 0x7f), new MIDI_event_ex_t(0xb0, 0x26, 0x00)));
        surface->AddWidget(new PushButton_CSurfWidget("Select19", surface,   new MIDI_event_ex_t(0xb0, 0x27, 0x7f), new MIDI_event_ex_t(0xb0, 0x27, 0x00)));
        surface->AddWidget(new PushButton_CSurfWidget("Select20", surface,   new MIDI_event_ex_t(0xb0, 0x28, 0x7f), new MIDI_event_ex_t(0xb0, 0x28, 0x00)));

        surface->AddWidget(new PushButton_CSurfWidget("TrackGroup", surface, new MIDI_event_ex_t(0xb0, 0x7b, 0x7f), new MIDI_event_ex_t(0xb0, 0x7b, 0x00)));
        surface->AddWidget(new PushButton_CSurfWidget("TrackCopy", surface,  new MIDI_event_ex_t(0xb0, 0x78, 0x7f), new MIDI_event_ex_t(0xb0, 0x78, 0x00)));

        surface->AddWidget(new PushButton_CSurfWidget(SendsMode, surface, channel, "",   new MIDI_event_ex_t(0xb0, 0x68, 0x7f), new MIDI_event_ex_t(0xb0, 0x68, 0x00)));
*/
        
        
        surface->AddWidget(new PushButtonWithLatch_MidiWidget("", surface, "Order",             new MIDI_event_ex_t(0xb0, 0x0e, 0x7f), new MIDI_event_ex_t(0xb0, 0x0e, 0x00)));
        surface->AddWidget(new PushButtonWithLatch_MidiWidget("", surface, "ExternalSidechain", new MIDI_event_ex_t(0xb0, 0x11, 0x7f), new MIDI_event_ex_t(0xb0, 0x11, 0x00)));

        // Input
        surface->AddWidget(new PushButtonWithLatch_MidiWidget("", surface, "FiltersToCompressor",   new MIDI_event_ex_t(0xb0, 0x3d, 0x7f), new MIDI_event_ex_t(0xb0, 0x3d, 0x00)));
        surface->AddWidget(new PushButtonWithLatch_MidiWidget("", surface, "PhaseInvert",           new MIDI_event_ex_t(0xb0, 0x6c, 0x7f), new MIDI_event_ex_t(0xb0, 0x6c, 0x00)));
        surface->AddWidget(new PushButtonWithLatch_MidiWidget("", surface, "Preset",                new MIDI_event_ex_t(0xb0, 0x3a, 0x7f), new MIDI_event_ex_t(0xb0, 0x3a, 0x00)));

        //surface->AddWidget(new Fader8Bit_CSurfWidget("HiCut", surface, channel,      new MIDI_event_ex_t(0xb0, 0x69, 0x7f)));
        //surface->AddWidget(new Fader8Bit_CSurfWidget("LoCut", surface, channel,      new MIDI_event_ex_t(0xb0, 0x67, 0x7f)));
    
        
        // Shape
        surface->AddWidget(new PushButtonWithLatch_MidiWidget("", surface, "Shape",     new MIDI_event_ex_t(0xb0, 0x35, 0x7f), new MIDI_event_ex_t(0xb0, 0x35, 0x00)));
        surface->AddWidget(new PushButtonWithLatch_MidiWidget("", surface, "HardGate",  new MIDI_event_ex_t(0xb0, 0x3b, 0x7f), new MIDI_event_ex_t(0xb0, 0x3b, 0x00)));

        //surface->AddWidget(new Fader8Bit_CSurfWidget("Gate", surface, channel,           new MIDI_event_ex_t(0xb0, 0x36, 0x7f)));
        //surface->AddWidget(new Fader8Bit_CSurfWidget("GateRelease", surface, channel,    new MIDI_event_ex_t(0xb0, 0x38, 0x7f)));
        //surface->AddWidget(new Fader8Bit_CSurfWidget("Sustain", surface, channel,        new MIDI_event_ex_t(0xb0, 0x37, 0x7f)));
        //surface->AddWidget(new Fader8Bit_CSurfWidget("Punch", surface, channel,          new MIDI_event_ex_t(0xb0, 0x39, 0x7f)l));

        //surface->AddWidget(new VUMeter_CSurfWidget(GateMeter, surface, channel, new  MIDI_event_ex_t(0xb0, 0x72, 0x7f)));
        
        // EQ
        surface->AddWidget(new PushButtonWithLatch_MidiWidget("", surface, Equalizer,    new MIDI_event_ex_t(0xb0, 0x50, 0x7f), new MIDI_event_ex_t(0xb0, 0x50, 0x00)));
        surface->AddWidget(new PushButtonWithLatch_MidiWidget("", surface, LoCurve,      new MIDI_event_ex_t(0xb0, 0x5d, 0x7f), new MIDI_event_ex_t(0xb0, 0x5d, 0x00)));
        surface->AddWidget(new PushButtonWithLatch_MidiWidget("", surface, HiCurve,      new MIDI_event_ex_t(0xb0, 0x41, 0x7f), new MIDI_event_ex_t(0xb0, 0x41, 0x00)));

        surface->AddWidget(new Fader7Bit_MidiWidget("", surface, HiGain,       new MIDI_event_ex_t(0xb0, 0x52, 0x7f), new MIDI_event_ex_t(0xb0, 0x52, 0x00)));
        surface->AddWidget(new Fader7Bit_MidiWidget("", surface, HiFrequency,  new MIDI_event_ex_t(0xb0, 0x53, 0x7f), new MIDI_event_ex_t(0xb0, 0x53, 0x00)));
        
        surface->AddWidget(new Fader7Bit_MidiWidget("", surface, HiMidGain,         new MIDI_event_ex_t(0xb0, 0x55, 0x7f), new MIDI_event_ex_t(0xb0, 0x55, 0x00)));
        surface->AddWidget(new Fader7Bit_MidiWidget("", surface, HiMidFrequency,    new MIDI_event_ex_t(0xb0, 0x56, 0x7f), new MIDI_event_ex_t(0xb0, 0x56, 0x00)));
        surface->AddWidget(new Fader7Bit_MidiWidget("", surface, HiMidQ,            new MIDI_event_ex_t(0xb0, 0x57, 0x7f), new MIDI_event_ex_t(0xb0, 0x57, 0x00)));
        
        surface->AddWidget(new Fader7Bit_MidiWidget("", surface, LoMidGain,         new MIDI_event_ex_t(0xb0, 0x58, 0x7f), new MIDI_event_ex_t(0xb0, 0x58, 0x00)));
        surface->AddWidget(new Fader7Bit_MidiWidget("", surface, LoMidFrequency,    new MIDI_event_ex_t(0xb0, 0x59, 0x7f), new MIDI_event_ex_t(0xb0, 0x59, 0x00)));
        surface->AddWidget(new Fader7Bit_MidiWidget("", surface, LoMidQ,            new MIDI_event_ex_t(0xb0, 0x5a, 0x7f), new MIDI_event_ex_t(0xb0, 0x5a, 0x00)));
        
        surface->AddWidget(new Fader7Bit_MidiWidget("", surface, LoGain,        new MIDI_event_ex_t(0xb0, 0x5b, 0x7f), new MIDI_event_ex_t(0xb0, 0x5b, 0x00)));
        surface->AddWidget(new Fader7Bit_MidiWidget("", surface, LoFrequency,   new MIDI_event_ex_t(0xb0, 0x5c, 0x7f), new MIDI_event_ex_t(0xb0, 0x5c, 0x00)));

        // Compressor
        surface->AddWidget(new PushButtonWithLatch_MidiWidget("", surface, Compressor, 1,   new MIDI_event_ex_t(0xb0, 0x2e, 0x7f), new MIDI_event_ex_t(0xb0, 0x2e, 0x00)));

        surface->AddWidget(new Fader7Bit_MidiWidget("", surface, Threshold,        new MIDI_event_ex_t(0xb0, 0x2f, 0x7f), new MIDI_event_ex_t(0xb0, 0x2f, 0x00)));
        surface->AddWidget(new Fader7Bit_MidiWidget("", surface, Release,          new MIDI_event_ex_t(0xb0, 0x30, 0x7f), new MIDI_event_ex_t(0xb0, 0x30, 0x00)));
        surface->AddWidget(new Fader7Bit_MidiWidget("", surface, Ratio,            new MIDI_event_ex_t(0xb0, 0x31, 0x7f), new MIDI_event_ex_t(0xb0, 0x31, 0x00)));
        surface->AddWidget(new Fader7Bit_MidiWidget("", surface, Parallel,         new MIDI_event_ex_t(0xb0, 0x32, 0x7f), new MIDI_event_ex_t(0xb0, 0x32, 0x00)));
        surface->AddWidget(new Fader7Bit_MidiWidget("", surface, Attack,           new MIDI_event_ex_t(0xb0, 0x33, 0x7f), new MIDI_event_ex_t(0xb0, 0x33, 0x00)));
        
        surface->AddWidget(new GainReductionMeter_MidiWidget("", surface, CompressorMeter, 0.0, -20.0, new  MIDI_event_ex_t(0xb0, 0x73, 0x7f), new  MIDI_event_ex_t(0xb0, 0x73, 0x00)));

        // Output

        surface->AddWidget(new Fader7Bit_MidiWidget("", surface, Drive,            new MIDI_event_ex_t(0xb0, 0x0f, 0x7f), new MIDI_event_ex_t(0xb0, 0x0f, 0x00)));
        surface->AddWidget(new Fader7Bit_MidiWidget("", surface, Character,        new MIDI_event_ex_t(0xb0, 0x12, 0x7f), new MIDI_event_ex_t(0xb0, 0x12, 0x00)));

        // Channel
        channel = new RealSurfaceChannel( "", surface);
        surface->AddChannel(channel);

        //channel->AddWidget(new Fader8Bit_CSurfWidget("InputGain", surface, channel,  new MIDI_event_ex_t(0xb0, 0x6b, 0x7f)));
        
        channel->AddWidget(new VUMeter_MidiWidget("", surface, ChannelInputMeterLeft, -60.0, 6.0, new  MIDI_event_ex_t(0xb0, 0x6e, 0x7f),     new MIDI_event_ex_t(0xb0, 0x6e, 0x00)));
        channel->AddWidget(new VUMeter_MidiWidget("", surface, ChannelInputMeterRight, -60.0, 6.0, new  MIDI_event_ex_t(0xb0, 0x6f, 0x7f),    new  MIDI_event_ex_t(0xb0, 0x6f, 0x00)));

        channel->AddWidget(new PushButtonWithLatch_MidiWidget("", surface, Solo,       new MIDI_event_ex_t(0xb0, 0x0d, 0x7f), new MIDI_event_ex_t(0xb0, 0x0d, 0x00)));
        channel->AddWidget(new PushButtonWithLatch_MidiWidget("", surface, Mute,       new MIDI_event_ex_t(0xb0, 0x0c, 0x7f), new MIDI_event_ex_t(0xb0, 0x0c, 0x00)));

        channel->AddWidget(new Fader7Bit_MidiWidget("", surface, Fader,      new MIDI_event_ex_t(0xb0, 0x07, 0x7f),  new MIDI_event_ex_t(0xb0, 0x07, 0x00)));
        channel->AddWidget(new Fader7Bit_MidiWidget("", surface, Rotary,         new MIDI_event_ex_t(0xb0, 0x0a, 0x7f), new MIDI_event_ex_t(0xb0, 0x0a, 0x00)));
        

        channel->AddWidget(new VUMeter_MidiWidget("", surface, ChannelOutputMeterLeft, -60.0, 6.0, new  MIDI_event_ex_t(0xb0, 0x70, 0x7f),     new MIDI_event_ex_t(0xb0, 0x70, 0x00)));
        channel->AddWidget(new VUMeter_MidiWidget("", surface, ChannelOutputMeterRight, -60.0, 6.0, new  MIDI_event_ex_t(0xb0, 0x71, 0x7f),    new  MIDI_event_ex_t(0xb0, 0x71, 0x00)));
    }
    else
    {
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, "Track",       new MIDI_event_ex_t(0x90, 0x28, 0x7f), new MIDI_event_ex_t(0x90, 0x28, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, "Send",        new MIDI_event_ex_t(0x90, 0x29, 0x7f), new MIDI_event_ex_t(0x90, 0x29, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, "Pan",         new MIDI_event_ex_t(0x90, 0x2a, 0x7f), new MIDI_event_ex_t(0x90, 0x2a, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, "Plugin",      new MIDI_event_ex_t(0x90, 0x2b, 0x7f), new MIDI_event_ex_t(0x90, 0x2b, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, "EQ",          new MIDI_event_ex_t(0x90, 0x2c, 0x7f), new MIDI_event_ex_t(0x90, 0x2c, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, "Instrument",  new MIDI_event_ex_t(0x90, 0x2d, 0x7f), new MIDI_event_ex_t(0x90, 0x2d, 0x00)));

        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, "nameValue",   new MIDI_event_ex_t(0x90, 0x34, 0x7f), new MIDI_event_ex_t(0x90, 0x34, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, "smpteBeats",  new MIDI_event_ex_t(0x90, 0x35, 0x7f), new MIDI_event_ex_t(0x90, 0x35, 0x00)));
        
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, NextMap,       new MIDI_event_ex_t(0x90, 0x36, 0x7f), new MIDI_event_ex_t(0x90, 0x36, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, "F2",          new MIDI_event_ex_t(0x90, 0x37, 0x7f), new MIDI_event_ex_t(0x90, 0x37, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, "F3",          new MIDI_event_ex_t(0x90, 0x38, 0x7f), new MIDI_event_ex_t(0x90, 0x38, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, "F4",          new MIDI_event_ex_t(0x90, 0x39, 0x7f), new MIDI_event_ex_t(0x90, 0x39, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, "F5",          new MIDI_event_ex_t(0x90, 0x3a, 0x7f), new MIDI_event_ex_t(0x90, 0x3a, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, "F6",          new MIDI_event_ex_t(0x90, 0x3b, 0x7f), new MIDI_event_ex_t(0x90, 0x3b, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, UnlockTracks,  new MIDI_event_ex_t(0x90, 0x3c, 0x7f), new MIDI_event_ex_t(0x90, 0x3c, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, LockTracks,    new MIDI_event_ex_t(0x90, 0x3d, 0x7f), new MIDI_event_ex_t(0x90, 0x3d, 0x00)));

        surface->AddWidget(new PushButtonWithRelease_MidiWidget(RealControlSurface, surface, Shift,     new MIDI_event_ex_t(0x90, 0x46, 0x7f), new MIDI_event_ex_t(0x90, 0x46, 0x00)));
        surface->AddWidget(new PushButtonWithRelease_MidiWidget(RealControlSurface, surface, Option,    new MIDI_event_ex_t(0x90, 0x47, 0x7f), new MIDI_event_ex_t(0x90, 0x47, 0x00)));
        surface->AddWidget(new PushButtonWithRelease_MidiWidget(RealControlSurface, surface, Control,   new MIDI_event_ex_t(0x90, 0x48, 0x7f), new MIDI_event_ex_t(0x90, 0x48, 0x00)));
        surface->AddWidget(new PushButtonWithRelease_MidiWidget(RealControlSurface, surface, Alt,       new MIDI_event_ex_t(0x90, 0x49, 0x7f), new MIDI_event_ex_t(0x90, 0x49, 0x00)));
        
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, Read,          new MIDI_event_ex_t(0x90, 0x4a, 0x7f), new MIDI_event_ex_t(0x90, 0x4a, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, Write,         new MIDI_event_ex_t(0x90, 0x4b, 0x7f), new MIDI_event_ex_t(0x90, 0x4b, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, Trim,          new MIDI_event_ex_t(0x90, 0x4c, 0x7f), new MIDI_event_ex_t(0x90, 0x4c, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, Touch,         new MIDI_event_ex_t(0x90, 0x4d, 0x7f), new MIDI_event_ex_t(0x90, 0x4d, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, Latch,         new MIDI_event_ex_t(0x90, 0x4e, 0x7f), new MIDI_event_ex_t(0x90, 0x4e, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, Group,         new MIDI_event_ex_t(0x90, 0x4f, 0x7f), new MIDI_event_ex_t(0x90, 0x4f, 0x00)));
        
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, Save,          new MIDI_event_ex_t(0x90, 0x50, 0x7f), new MIDI_event_ex_t(0x90, 0x50, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, Undo,          new MIDI_event_ex_t(0x90, 0x51, 0x7f), new MIDI_event_ex_t(0x90, 0x51, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, Cancel,        new MIDI_event_ex_t(0x90, 0x52, 0x7f), new MIDI_event_ex_t(0x90, 0x52, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, Enter,         new MIDI_event_ex_t(0x90, 0x53, 0x7f), new MIDI_event_ex_t(0x90, 0x53, 0x00)));
        
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, Cycle,         new MIDI_event_ex_t(0x90, 0x56, 0x7f), new MIDI_event_ex_t(0x90, 0x56, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, "Drop",        new MIDI_event_ex_t(0x90, 0x57, 0x7f), new MIDI_event_ex_t(0x90, 0x57, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, "Replace",     new MIDI_event_ex_t(0x90, 0x58, 0x7f), new MIDI_event_ex_t(0x90, 0x58, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, Click,         new MIDI_event_ex_t(0x90, 0x59, 0x7f), new MIDI_event_ex_t(0x90, 0x59, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, "Solo",        new MIDI_event_ex_t(0x90, 0x5a, 0x7f), new MIDI_event_ex_t(0x90, 0x5a, 0x00)));

        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, Up,            new MIDI_event_ex_t(0x90, 0x60, 0x7f), new MIDI_event_ex_t(0x90, 0x60, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, Down,          new MIDI_event_ex_t(0x90, 0x61, 0x7f), new MIDI_event_ex_t(0x90, 0x61, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, Left,          new MIDI_event_ex_t(0x90, 0x62, 0x7f), new MIDI_event_ex_t(0x90, 0x62, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, Right,         new MIDI_event_ex_t(0x90, 0x63, 0x7f), new MIDI_event_ex_t(0x90, 0x63, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, Zoom,          new MIDI_event_ex_t(0x90, 0x64, 0x7f), new MIDI_event_ex_t(0x90, 0x64, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, Scrub,         new MIDI_event_ex_t(0x90, 0x65, 0x7f), new MIDI_event_ex_t(0x90, 0x65, 0x00)));

        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, BankLeft,      new MIDI_event_ex_t(0x90, 0x2e, 0x7f), new MIDI_event_ex_t(0x90, 0x2e, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, BankRight,     new MIDI_event_ex_t(0x90, 0x2f, 0x7f), new MIDI_event_ex_t(0x90, 0x2f, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, ChannelLeft,   new MIDI_event_ex_t(0x90, 0x30, 0x7f), new MIDI_event_ex_t(0x90, 0x30, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, ChannelRight,  new MIDI_event_ex_t(0x90, 0x31, 0x7f), new MIDI_event_ex_t(0x90, 0x31, 0x00)));

        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, Marker,        new MIDI_event_ex_t(0x90, 0x54, 0x7f), new MIDI_event_ex_t(0x90, 0x54, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, Nudge,         new MIDI_event_ex_t(0x90, 0x55, 0x7f), new MIDI_event_ex_t(0x90, 0x55, 0x00)));

        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, Rewind,        new MIDI_event_ex_t(0x90, 0x5b, 0x7f), new MIDI_event_ex_t(0x90, 0x5b, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, FastForward,   new MIDI_event_ex_t(0x90, 0x5c, 0x7f), new MIDI_event_ex_t(0x90, 0x5c, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, Stop,          new MIDI_event_ex_t(0x90, 0x5d, 0x7f), new MIDI_event_ex_t(0x90, 0x5d, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, Play,          new MIDI_event_ex_t(0x90, 0x5e, 0x7f), new MIDI_event_ex_t(0x90, 0x5e, 0x00)));
        surface->AddWidget(new PushButton_MidiWidget(RealControlSurface, surface, Record,        new MIDI_event_ex_t(0x90, 0x5f, 0x7f), new MIDI_event_ex_t(0x90, 0x5f, 0x00)));
        
        for(int i = 0; i < surface->GetNumBankableChannels(); ++i)
        {
            channel = new RealSurfaceChannel(to_string(i + 1), surface);
            surface->AddChannel(channel);
            
            channel->AddWidget(new PushButtonWithRelease_MidiWidget("", surface, FaderTouch, new MIDI_event_ex_t(0x90, 0x68 + i, 0x7f), new MIDI_event_ex_t(0x90, 0x68 + i, 0x00)));
            
            channel->AddWidget(new PushButtonCycler_MidiWidget("", surface, RotaryPush, new MIDI_event_ex_t(0x90, 0x20 + i, 0x7f), new MIDI_event_ex_t(0x90, 0x20 + i, 0x00)));
            channel->AddWidget(new Encoder_MidiWidget("", surface, Rotary, new MIDI_event_ex_t(0xb0, 0x10 + i, 0x7f), new MIDI_event_ex_t(0xb0, 0x10 + i, 0x00)));

            channel->AddWidget(new Display_MidiWidget("", surface, Display, i));
            channel->AddWidget(new Fader14Bit_MidiWidget("", surface, Fader, -72.0, 12.0, new MIDI_event_ex_t(0xe0 + i, 0x7f, 0x7f), new MIDI_event_ex_t(0xe0 + i, 0x00, 0x00)));
            channel->AddWidget(new PushButton_MidiWidget("", surface, RecordArm,   new MIDI_event_ex_t(0x90, 0x00 + i, 0x7f), new MIDI_event_ex_t(0x90, 0x00 + i, 0x00)));
            channel->AddWidget(new PushButton_MidiWidget("", surface, Solo,        new MIDI_event_ex_t(0x90, 0x08 + i, 0x7f), new MIDI_event_ex_t(0x90, 0x08 + i, 0x00)));
            channel->AddWidget(new PushButton_MidiWidget("", surface, Mute,        new MIDI_event_ex_t(0x90, 0x10 + i, 0x7f), new MIDI_event_ex_t(0x90, 0x10 + i, 0x00)));
            channel->AddWidget(new PushButton_MidiWidget("", surface, Select,      new MIDI_event_ex_t(0x90, 0x18 + i, 0x7f), new MIDI_event_ex_t(0x90, 0x18 + i, 0x00)));
        }
    }
}
