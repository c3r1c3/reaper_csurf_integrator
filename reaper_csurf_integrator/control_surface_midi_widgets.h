//
//  control_surface_midi_widgets.h
//  reaper_csurf_integrator
//
//

#ifndef control_surface_midi_widgets_h
#define control_surface_midi_widgets_h

#include "control_surface_integrator.h"
#include "handy_reaper_functions.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class PushButton_MidiWidget : public MidiWidget
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
protected:
    int reverseSense_ = 0;
    
public:
    PushButton_MidiWidget(string name, RealCSurf* surface, CSurfChannel* channel, string GUID, MIDI_event_ex_t* press, MIDI_event_ex_t* release) : MidiWidget(name, surface, channel, GUID, press, release)  {}
    
    PushButton_MidiWidget(string name, RealCSurf* surface, CSurfChannel* channel, string GUID, int reverseSense, MIDI_event_ex_t* press, MIDI_event_ex_t* release) : MidiWidget(name, surface, channel, GUID, press, release), reverseSense_(reverseSense) {}
    
    void SetValue(double value) override
    {
        GetSurface()->SendMidiMessage(value == reverseSense_ ? GetMidiReleaseMessage() : GetMidiPressMessage());
    }
    
    virtual void SetValueToZero() override
    {
        SetValue(reverseSense_ ? 1 : 0);
    }
    
    virtual void ProcessMidiMessage(const MIDI_event_ex_t* midiMessage) override
    {
        if(GetMidiPressMessage()->IsEqualTo(midiMessage))
            GetSurface()->GetManager()->RunAction(GetGUID(), GetName(), reverseSense_ ? 0 : 1);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class PushButtonWithRelease_MidiWidget : public PushButton_MidiWidget
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
public:
    PushButtonWithRelease_MidiWidget(string name, RealCSurf* surface, CSurfChannel* channel, string GUID, MIDI_event_ex_t* press, MIDI_event_ex_t* release) : PushButton_MidiWidget(name, surface, channel, GUID, press, release)  {}
    
    PushButtonWithRelease_MidiWidget(string name, RealCSurf* surface, CSurfChannel* channel, string GUID, int reverseSense, MIDI_event_ex_t* press, MIDI_event_ex_t* release) : PushButton_MidiWidget(name, surface, channel, GUID, reverseSense, press, release) {}
    
    virtual void ProcessMidiMessage(const MIDI_event_ex_t* midiMessage) override
    {
        if(GetMidiPressMessage()->IsEqualTo(midiMessage))
            GetSurface()->GetManager()->RunAction(GetGUID(), GetName(), reverseSense_ ? 0 : 1);
        else if(GetMidiReleaseMessage()->IsEqualTo(midiMessage))
            GetSurface()->GetManager()->RunAction(GetGUID(), GetName(), reverseSense_ ? 1 : 0);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Fader14Bit_MidiWidget : public MidiWidget
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
private:
    double minDB_ = 0.0;
    double maxDB_ = 0.0;
    
public:
    Fader14Bit_MidiWidget(string name, RealCSurf* surface, CSurfChannel* channel, string GUID, double minDB, double maxDB, MIDI_event_ex_t* press, MIDI_event_ex_t* release) : MidiWidget(name, surface, channel, GUID, press, release), minDB_(minDB), maxDB_(maxDB) {}
    
    double GetMinDB() override { return minDB_; }

    double GetMaxDB() override { return maxDB_; }

    virtual void SetValue(double volume) override
    {
        int volint = normalizedToInt14(volume);
/*
        if( ! Surface()->Surface()->IsFlipped())
        {
            double volumeDB = VAL2DB(normalizedToVol(volume));
            
            //slope = (output_end - output_start) / (input_end - input_start)
            //output = output_start + slope * (input - input_start)
            
            // Map Reaper Fader range to surface Fader range
            double slope = (GetMaxDB() - GetMinDB()) / (Surface()->Manager()->GetFaderMaxDB() - Surface()->Manager()->GetFaderMinDB());
            double output = GetMinDB() + slope * (volumeDB - Surface()->Manager()->GetFaderMinDB());

            output = volToNormalized(DB2VAL(output));
            
            volint = normalizedToInt14(output);
        }
*/
        GetSurface()->SendMidiMessage(GetMidiPressMessage()->midi_message[0], volint&0x7f, (volint>>7)&0x7f);
    }
    
    virtual void SetValue(double volume, int displayMode) override
    {
        SetValue(volume);
    }
    
    virtual void SetValueToZero() override
    {
        SetValue(0.0);
    }
    
    virtual void ProcessMidiMessage(const MIDI_event_ex_t* midiMessage) override
    {
        if(midiMessage->midi_message[0] == GetMidiPressMessage()->midi_message[0])
        {
            double volume = int14ToNormalized(midiMessage->midi_message[2], midiMessage->midi_message[1]);
            
            if( ! GetSurface()->GetLogicalSurface()->IsFlipped())
            {
                
                //volume *= (GetMaxDB() - GetMinDB()) / (Surface()->Manager()->GetFaderMaxDB() - Surface()->Manager()->GetFaderMinDB());
                
                //volume > 1.0 ? 1.0 : volume;

                /*
                double volumeDB = volume * (GetMaxDB() - GetMinDB());
                
                //slope = (output_end - output_start) / (input_end - input_start)
                //output = output_start + slope * (input - input_start)
                
                // Map Surface Fader range to Reaper Fader range
                double slope = (Surface()->Manager()->GetFaderMaxDB() - Surface()->Manager()->GetFaderMinDB()) / (GetMaxDB() - GetMinDB());
                double output = Surface()->Manager()->GetFaderMinDB() + slope * (volumeDB - GetMinDB());
                
                volume = volToNormalized(DB2VAL(output));
                 */
            }
            
            GetSurface()->GetManager()->RunAction(GetGUID(), GetName(), volume);
        }
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Fader8Bit_MidiWidget : public MidiWidget
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
public:
    Fader8Bit_MidiWidget(string name, RealCSurf* surface, CSurfChannel* channel, string GUID, MIDI_event_ex_t* press, MIDI_event_ex_t* release) : MidiWidget(name, surface, channel, GUID, press, release) {}
    
    virtual void SetValue(double value) override
    {
        GetSurface()->SendMidiMessage(GetMidiPressMessage()->midi_message[0], GetMidiPressMessage()->midi_message[1], normalizedToUchar(value));
    }
    
    virtual void SetValue(double value, int displayMode) override
    {
        SetValue(value);
    }
    
    virtual void SetValueToZero() override
    {
        SetValue(0.0, 0x00);
    }
    
    virtual void ProcessMidiMessage(const MIDI_event_ex_t* midiMessage) override
    {
        if(midiMessage->midi_message[0] == GetMidiPressMessage()->midi_message[0] && midiMessage->midi_message[1] == GetMidiPressMessage()->midi_message[1])
            GetSurface()->GetManager()->RunAction(GetGUID(), GetName(), ucharToNormalized(midiMessage->midi_message[2]));
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Encoder_MidiWidget : public MidiWidget
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
public:
    Encoder_MidiWidget(string name, RealCSurf* surface, CSurfChannel* channel, string GUID, MIDI_event_ex_t* press, MIDI_event_ex_t* release) : MidiWidget(name, surface, channel, GUID, press, release) {}
    
    virtual void SetValue(double pan, int displayMode) override
    {
        unsigned char panch = normalizedToUchar(pan);
        
        int val = (1+((panch*11)>>7)) | displayMode; // display modes -- 0x00 = line (e.g. pan), 0x01 = boost/cut (e.g. eq), 0x02 = fill from right (e.g. level), 0x03 = center fill (e.g. pan width)
        
        GetSurface()->SendMidiMessage(GetMidiPressMessage()->midi_message[0], GetMidiPressMessage()->midi_message[1] + 0x20, val);
    }
    
    virtual void SetValueToZero() override
    {
        SetValue(0.0, 0x00);
    }
    
    virtual void ProcessMidiMessage(const MIDI_event_ex_t* midiMessage) override
    {
        if(midiMessage->midi_message[0] == GetMidiPressMessage()->midi_message[0] && midiMessage->midi_message[1] == GetMidiPressMessage()->midi_message[1])
        {
            double value = (midiMessage->midi_message[2] & 0x3f) / 63.0;
            
            if (midiMessage->midi_message[2] & 0x40)
                value = -value;
            
            value += GetSurface()->GetManager()->GetCurrentNormalizedValue(GetGUID(), GetName());
            
            GetSurface()->GetManager()->RunAction(GetGUID(), GetName(), value);
        }
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EncoderCycledAction_MidiWidget : public Encoder_MidiWidget
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
private:
    MIDI_event_ex_t* cycle_;
    
public:
    EncoderCycledAction_MidiWidget(string name, RealCSurf* surface, CSurfChannel* channel, string GUID, MIDI_event_ex_t* press, MIDI_event_ex_t* release, MIDI_event_ex_t* cycle) : Encoder_MidiWidget(name, surface, channel, GUID, press, release), cycle_(cycle) {}
    
    virtual void ProcessMidiMessage(const MIDI_event_ex_t* midiMessage) override
    {
        if(midiMessage->IsEqualTo(cycle_))
            GetSurface()->GetManager()->CycleAction(GetGUID(), GetName());
        
        Encoder_MidiWidget::ProcessMidiMessage(midiMessage);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class VUMeter_MidifWidget : public MidiWidget
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
private:
    double minDB_ = 0.0;
    double maxDB_ = 0.0;

public:
    VUMeter_MidifWidget(string name, RealCSurf* surface, CSurfChannel* channel, string GUID, double minDB, double maxDB, MIDI_event_ex_t* press, MIDI_event_ex_t* release) : MidiWidget(name, surface, channel, GUID, press, release), minDB_(minDB), maxDB_(maxDB){}
    
    double GetMinDB() override { return minDB_; }
    
    double GetMaxDB() override { return maxDB_; }
    
    virtual void SetValueToZero() override
    {
        SetValue(GetSurface()->GetManager()->GetVUMinDB());
    }
    
    void SetValue(double value) override
    {
        //slope = (output_end - output_start) / (input_end - input_start)
        //output = output_start + slope * (input - input_start)
        
        // First, map Reaper VU range to surface VU range
        double slope = (GetMaxDB() - GetMinDB()) / (GetSurface()->GetManager()->GetVUMaxDB() - GetSurface()->GetManager()->GetVUMinDB());
        double output = GetMinDB() + slope * (value - GetSurface()->GetManager()->GetVUMinDB());

        // Now map surface VU range to widget range
        slope = 127.0 / (GetMaxDB() - GetMinDB());
        output = slope * (output - GetMinDB());
        
        GetSurface()->SendMidiMessage(GetMidiPressMessage()->midi_message[0], GetMidiPressMessage()->midi_message[1], output);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Display_MidiWidget : public MidiWidget
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
    int slotIndex_ = 0;
    
public:
    Display_MidiWidget(string name, RealCSurf* surface, CSurfChannel* channel, int slotIndex, string GUID) : MidiWidget(name, surface, channel, GUID, new MIDI_event_ex_t(0x00, 0x00, 0x00), new MIDI_event_ex_t(0x00, 0x00, 0x00)), slotIndex_(slotIndex) {}
    
    virtual void SetValueToZero() override
    {
        SetValue("");
    }
    
    void SetValue(string displayText) override
    {
        int pad = 7;
        const char* text = displayText.c_str();
        
        struct
        {
            MIDI_event_ex_t evt;
            char data[512];
        } midiSysExData;
        midiSysExData.evt.frame_offset=0;
        midiSysExData.evt.size=0;
        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = 0xF0;
        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = 0x00;
        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = 0x00;
        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = 0x66;
        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = 0x14;
        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = 0x12;
        
        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = slotIndex_ * 7;
        
        int l = strlen(text);
        if (pad < l)
            l = pad;
        if (l > 200)
            l = 200;
        
        int cnt = 0;
        while (cnt < l)
        {
            midiSysExData.evt.midi_message[midiSysExData.evt.size++] = *text++;
            cnt++;
        }
        
        while (cnt++ < pad)
            midiSysExData.evt.midi_message[midiSysExData.evt.size++] = ' ';
        
        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = 0xF7;
        
        GetSurface()->SendMidiMessage(&midiSysExData.evt);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class MidiCSurf : public RealCSurf
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
private:
    midi_Input* midiInput_ = nullptr;
    midi_Output* midiOutput_ = nullptr;
    
    bool midiInMonitor_ = false;
    bool midiOutMonitor_ = false;

    void HandleMidiInput()
    {
        if(midiInput_)
        {
            GetManager()->GetDAW()->SwapBufsPrecise(midiInput_);
            MIDI_eventlist* list = midiInput_->GetReadBuf();
            int bpos = 0;
            MIDI_event_t* evt;
            while ((evt = list->EnumItems(&bpos)))
                ProcessMidiMessage((MIDI_event_ex_t*)evt);
        }
    }
    
    void ProcessMidiMessage(const MIDI_event_ex_t* evt)
    {
        for(auto & channel : GetChannels())
            channel->ProcessMidiMessage(evt);
        
        if(midiInMonitor_)
        {
            char buffer[250];
            sprintf(buffer, "IN -> %s %02x  %02x  %02x \n", GetName().c_str(), evt->midi_message[0], evt->midi_message[1], evt->midi_message[2]);
            GetManager()->GetDAW()->ShowConsoleMsg(buffer);
        }
    }
    
public:
    virtual ~MidiCSurf()
    {
        if (midiInput_) delete midiInput_;
        if(midiOutput_) delete midiOutput_;
    }
    
    MidiCSurf(LogicalSurface* logicalSurface, const string name, const int numSurfaceChannels, midi_Input* midiInput, midi_Output* midiOutput, bool midiInMonitor, bool midiOutMonitor)
    : RealCSurf(logicalSurface, name, numSurfaceChannels), midiInput_(midiInput), midiOutput_(midiOutput), midiInMonitor_(midiInMonitor), midiOutMonitor_(midiOutMonitor) {}
    
    virtual void SendMidiMessage(MIDI_event_ex_t* midiMessage) override
    {
        if(midiOutput_)
            midiOutput_->SendMsg(midiMessage, -1);
    }
    
    virtual void SendMidiMessage(int first, int second, int third) override
    {
        if(midiOutput_)
            midiOutput_->Send(first, second, third, -1);
        
        if(midiOutMonitor_)
        {
            char buffer[250];
            sprintf(buffer, "OUT -> %s %02x  %02x  %02x \n", GetName().c_str(), first, second, third);
            GetManager()->GetDAW()->ShowConsoleMsg(buffer);
        }
    }
    
    virtual void RunAndUpdate() override
    {
        HandleMidiInput();
        RealCSurf::Update();
    }
};

#endif /* control_surface_midi_widgets_h */