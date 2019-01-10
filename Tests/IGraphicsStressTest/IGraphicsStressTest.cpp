#include "IGraphicsStressTest.h"
#include "IPlug_include_in_plug_src.h"

#include "IControl.h"

IGraphicsStressTest::IGraphicsStressTest(IPlugInstanceInfo instanceInfo)
: IPLUG_CTOR(kNumParams, 1, instanceInfo)
{
#if IPLUG_EDITOR
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };
  
#endif
}

void IGraphicsStressTest::LayoutUI(IGraphics* pGraphics)
{
  IRECT bounds = pGraphics->GetBounds();
  
  if(pGraphics->NControls()) {
    pGraphics->GetControl(0)->SetTargetAndDrawRECTs(bounds);
    pGraphics->GetControl(1)->SetTargetAndDrawRECTs(bounds);
    pGraphics->GetControlWithTag(kCtrlTagNumThings)->SetTargetAndDrawRECTs(bounds.GetGridCell(0, 2, 1));
    pGraphics->GetControlWithTag(kCtrlTagTestNum)->SetTargetAndDrawRECTs(bounds.GetGridCell(1, 2, 1));
    
    return;
  }
  
  pGraphics->SetSizeConstraints(100, 100000, 100, 100000);
  pGraphics->ShowFPSDisplay(true);
  pGraphics->AttachCornerResizer(EUIResizerMode::kUIResizerSize, true);
  pGraphics->SetKeyHandlerFunc([&](const IKeyPress& key)
  {
    switch (key.VK) {
      case VK_UP: mNumberOfThings++; break;
      case VK_DOWN: mNumberOfThings--; break;
      case VK_TAB: key.S ? mKindOfThing-- : mKindOfThing++; break;
      default: return false;
    }

    dynamic_cast<ITextControl*>(GetUI()->GetControlWithTag(kCtrlTagNumThings))->SetStrFmt(64, "Number of things = %i", mNumberOfThings);
    dynamic_cast<ITextControl*>(GetUI()->GetControlWithTag(kCtrlTagTestNum))->SetStrFmt(64, "Test %i/%i", mKindOfThing, 32);
    this->GetUI()->SetAllControlsDirty();
    return true;
  });
  
  pGraphics->HandleMouseOver(false);
  pGraphics->LoadFont(ROBOTTO_FN);
  pGraphics->AttachPanelBackground(COLOR_GRAY);
  pGraphics->AttachControl(new ILambdaControl(*this, bounds, [&](ILambdaControl* pCaller, IGraphics& g, IRECT& r)
  {
    static IBitmap smiley = g.LoadBitmap(SMILEY_FN);
    static ISVG tiger = g.LoadSVG(TIGER_FN);
    
    if(mKindOfThing == 0)
      g.DrawText(IText(40), "Press tab to go to next test, up/down to change the # of things", r);
    
    //      if (!g.CheckLayer(pCaller->mLayer))
    {
      //        g.StartLayer(r);
      
      for (int i=0; i<this->mNumberOfThings; i++)
      {
        IRECT rr = r.GetRandomSubRect();
        IColor rc = IColor::GetRandomColor();
        IBlend rb = {};
        static bool dir = 0;
        static float thickness = 5.f;
        static float roundness = 5.f;
        float rrad1 = rand() % 360;
        float rrad2 = rand() % 360;
        
        switch (mKindOfThing)
        {
          case 1:  g.DrawRect(rc, rr, &rb); break;
          case 2:  g.FillRect(rc, rr, &rb); break;
          case 3:  g.DrawRoundRect(rc, rr, roundness, &rb); break;
          case 4:  g.FillRoundRect(rc, rr, roundness, &rb); break;
          case 5:  g.DrawEllipse(rc, rr, &rb); break;
          case 6:  g.FillEllipse(rc, rr, &rb); break;
          case 7:  g.DrawArc(rc, rr.MW(), rr.MH(), rr.W() > rr.H() ? rr.H() : rr.W(), rrad1, rrad2, &rb,thickness); break;
          case 8:  g.FillArc(rc, rr.MW(), rr.MH(), rr.W() > rr.H() ? rr.H() : rr.W(), rrad1, rrad2, &rb); break;
          case 9:  g.DrawLine(rc, dir == 0 ? rr.L : rr.R, rr.B, dir == 0 ? rr.R : rr.L, rr.T, &rb,thickness); break;
          case 10: g.DrawDottedLine(rc, dir == 0 ? rr.L : rr.R, rr.B, dir == 0 ? rr.R : rr.L, rr.T, &rb, thickness); break;
          case 11: g.DrawFittedBitmap(smiley, rr, &rb); break;
          case 12: g.DrawSVG(tiger, rr); break;
          default:
            break;
        }
        
        dir = !dir;
      }
      //        pCaller->mLayer = g.EndLayer();
    }
    
    //      g.DrawLayer(pCaller->mLayer);
    
  }, 10000, false, false));
  
  pGraphics->AttachControl(new ITextControl(*this, bounds.GetGridCell(0, 2, 1), "", IText(100)), kCtrlTagNumThings);
  pGraphics->AttachControl(new ITextControl(*this, bounds.GetGridCell(1, 2, 1), "", IText(100)), kCtrlTagTestNum);
}
