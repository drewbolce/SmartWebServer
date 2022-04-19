// -----------------------------------------------------------------------------------
// Settings

#include "Settings.h"

void processSettingsGet();

void handleSettings() {
  char temp[240] = "";

  SERIAL_ONSTEP.setTimeout(webTimeout);
  onStep.serialRecvFlush();

  processSettingsGet();
  
  www.setContentLength(CONTENT_LENGTH_UNKNOWN);
  www.sendHeader("Cache-Control", "no-cache");
  www.send(200, "text/html", String());
 
  String data = FPSTR(html_headB);
  data.concat(FPSTR(html_main_cssB));
  data.concat(FPSTR(html_main_css1));
  data.concat(FPSTR(html_main_css2));
  data.concat(FPSTR(html_main_css3));
  data.concat(FPSTR(html_main_css4));
  data.concat(FPSTR(html_main_css5));
  www.sendContentAndClear(data);
  data.concat(FPSTR(html_main_css6));
  data.concat(FPSTR(html_main_css7));
  data.concat(FPSTR(html_main_css8));
  data.concat(FPSTR(html_main_css_btns1));
  www.sendContentAndClear(data);
  data.concat(FPSTR(html_main_css_btns2));
  data.concat(FPSTR(html_main_css_btns3));
  data.concat(FPSTR(html_main_cssE));
  data.concat(FPSTR(html_headE));

  data.concat(FPSTR(html_bodyB));
  www.sendContentAndClear(data);

  // scripts
  // active ajax page is: settingsAjax();
  data.concat("<script>var ajaxPage='settings.txt';</script>\n");
  data.concat(FPSTR(html_ajax_active));
  data.concat("<script>auto2Rate=2;</script>");
  sprintf_P(temp, html_ajaxScript, "settingsA.txt");
  data.concat(temp);

  // finish the standard http response header
  data.concat(FPSTR(html_onstep_header1));
  data.concat("OnStep");
  data.concat(FPSTR(html_onstep_header2));
  data.concat(firmwareVersion.str);
  data.concat(" (OnStep");
  if (status.getVersionStr(temp)) data.concat(temp); else data.concat("?");
  data.concat(FPSTR(html_onstep_header3));
  data.concat(FPSTR(html_linksStatN));
  data.concat(FPSTR(html_linksCtrlN));
  if (status.featureFound) data.concat(FPSTR(html_linksAuxN));
  data.concat(FPSTR(html_linksLibN));
  #if ENCODERS == ON
    data.concat(FPSTR(html_linksEncN));
  #endif
  www.sendContentAndClear(data);
  if (status.pecEnabled) data.concat(FPSTR(html_linksPecN));
  data.concat(FPSTR(html_linksSetS));
  data.concat(FPSTR(html_linksCfgN));
  data.concat(FPSTR(html_linksSetupN));
  data.concat(FPSTR(html_onstep_header4));
  www.sendContentAndClear(data);
 
  // OnStep wasn't found, show warning and info.
  if (!status.valid) {
    data.concat(FPSTR(html_bad_comms_message));
    www.sendContentAndClear(data);
    www.sendContent("");
    return;
  }

  data.concat("<div style='width: 35em;'>");

  data.concat(FPSTR(html_settingsStart));

  // Slew speed
  data.concat(FPSTR(html_settingsSlewSpeed1));
  data.concat(FPSTR(html_settingsSlewSpeed2));
  www.sendContentAndClear(data);

  if (status.mountType != MT_ALTAZM) {
    data.concat(FPSTR(html_settingsTrackComp1));
    data.concat(FPSTR(html_settingsTrackComp2));
    data.concat(FPSTR(html_settingsTrackComp3));
    www.sendContentAndClear(data);
  }

  data.concat(FPSTR(html_settingsTrack1));
  data.concat(FPSTR(html_settingsTrack2));
  
  data.concat(FPSTR(html_settingsPark1));
  www.sendContentAndClear(data);

  data.concat(FPSTR(html_settingsBuzzer1));
  data.concat(FPSTR(html_settingsBuzzer2));

  if (status.mountType == MT_GEM ||
     (status.getVersionMajor() >= 10 && status.mountType == MT_FORK)) {
    data.concat(FPSTR(html_settingsMFAuto1));
    data.concat(FPSTR(html_settingsMFAuto2));
    data.concat(FPSTR(html_settingsMFPause1));
    data.concat(FPSTR(html_settingsMFPause2));
  }

  data.concat(FPSTR(html_settingsEnd));

  data.concat("<br />");
  data.concat("</div></div></body></html>");
  
  www.sendContentAndClear(data);
  www.sendContent("");
}

void settingsAjaxGet() {
  www.setContentLength(CONTENT_LENGTH_UNKNOWN);
  www.sendHeader("Cache-Control", "no-cache");
  www.send(200, "text/plain", String());

  processSettingsGet();

  www.sendContent("");
}

void settingsAjax() {
  String data = "";

  www.setContentLength(CONTENT_LENGTH_UNKNOWN);
  www.sendHeader("Cache-Control", "no-cache");
  www.send(200, "text/plain", String());

  if (status.valid) {
    data.concat("bzr_on|");  if (status.buzzerEnabled) data.concat("disabled\n"); else data.concat("enabled\n");
    data.concat("bzr_off|"); if (status.buzzerEnabled) data.concat("enabled\n"); else data.concat("disabled\n");
    if (status.mountType == MT_GEM ||
       (status.getVersionMajor() >= 10 && status.mountType == MT_FORK)) {
      data.concat("mfa_on|");  if (status.autoMeridianFlips) data.concat("disabled\n"); else data.concat("enabled\n");
      data.concat("mfa_off|"); if (status.autoMeridianFlips) data.concat("enabled\n"); else data.concat("disabled\n");
      data.concat("mfp_on|");  if (status.pauseAtHome) data.concat("disabled\n"); else data.concat("enabled\n");
      data.concat("mfp_off|"); if (status.pauseAtHome) data.concat("enabled\n"); else data.concat("disabled\n");
    }
    if (status.mountType != MT_ALTAZM) {
      // RC_NONE, RC_REFR_RA, RC_REFR_BOTH, RC_FULL_RA, RC_FULL_BOTH
      if (status.rateCompensation == RC_NONE) {
        data.concat("ot_on|enabled\n");
        data.concat("ot_ref|enabled\n");
        data.concat("ot_off|disabled\n");
        data.concat("ot_dul|disabled\n");
        data.concat("ot_sgl|disabled\n");
      } else
      if (status.rateCompensation == RC_REFR_RA) {
        data.concat("ot_on|enabled\n");
        data.concat("ot_ref|disabled\n");
        data.concat("ot_off|enabled\n");
        data.concat("ot_dul|enabled\n");
        data.concat("ot_sgl|disabled\n");
      } else
      if (status.rateCompensation == RC_REFR_BOTH) {
        data.concat("ot_on|enabled\n");
        data.concat("ot_ref|disabled\n");
        data.concat("ot_off|enabled\n");
        data.concat("ot_dul|disabled\n");
        data.concat("ot_sgl|enabled\n");
      } else
      if (status.rateCompensation == RC_FULL_RA) {
        data.concat("ot_on|disabled\n");
        data.concat("ot_ref|enabled\n");
        data.concat("ot_off|enabled\n");
        data.concat("ot_dul|enabled\n");
        data.concat("ot_sgl|disabled\n");
      } else
      if (status.rateCompensation == RC_FULL_BOTH) {
        data.concat("ot_on|disabled\n");
        data.concat("ot_ref|enabled\n");
        data.concat("ot_off|enabled\n");
        data.concat("ot_dul|disabled\n");
        data.concat("ot_sgl|enabled\n");
      }
    }
  } else {
    data.concat("trk_on|disabled\n");
    data.concat("trk_off|disabled\n");
    data.concat("bzr_on|disabled\n");
    data.concat("bzr_off|disabled\n");
    data.concat("mfa_on|disabled\n");
    data.concat("mfa_off|disabled\n");
    data.concat("mfp_on|disabled\n");
    data.concat("mfp_off|disabled\n");
    data.concat("ot_on|disabled\n");
    data.concat("ot_ref|disabled\n");
    data.concat("ot_off|disabled\n");
    data.concat("ot_dul|disabled\n");
    data.concat("ot_sgl|disabled\n");
  }

  String temp = onStep.commandString(":GX92#");
  float nominalRate = temp.toFloat();
  temp = onStep.commandString(":GX93#");
  float currentRate = temp.toFloat();
  if (nominalRate > 0.001 && nominalRate < 180.0 && currentRate > 0.001 && currentRate < 180.0) {
    double rateRatio = currentRate/nominalRate;

    if (rateRatio > 1.75) {
      data.concat("sr_vf|disabled\n");
      data.concat("sr_f|enabled\n");
      data.concat("sr_n|enabled\n");
      data.concat("sr_s|enabled\n");
      data.concat("sr_vs|enabled\n");
    } else
    if (rateRatio > 1.25) {
      data.concat("sr_vf|enabled\n");
      data.concat("sr_f|disabled\n");
      data.concat("sr_n|enabled\n");
      data.concat("sr_s|enabled\n");
      data.concat("sr_vs|enabled\n");
    } else
    if (rateRatio > 0.875) {
      data.concat("sr_vf|enabled\n");
      data.concat("sr_f|enabled\n");
      data.concat("sr_n|disabled\n");
      data.concat("sr_s|enabled\n");
      data.concat("sr_vs|enabled\n");
    } else
    if (rateRatio > 0.625) {
      data.concat("sr_vf|enabled\n");
      data.concat("sr_f|enabled\n");
      data.concat("sr_n|enabled\n");
      data.concat("sr_s|disabled\n");
      data.concat("sr_vs|enabled\n");
    } else {
      data.concat("sr_vf|enabled\n");
      data.concat("sr_f|enabled\n");
      data.concat("sr_n|enabled\n");
      data.concat("sr_s|enabled\n");
      data.concat("sr_vs|disabled\n");
    }
  } else {
    data.concat("sr_vf|disabled\n");
    data.concat("sr_f|disabled\n");
    data.concat("sr_n|disabled\n");
    data.concat("sr_s|disabled\n");
    data.concat("sr_vs|disabled\n");
  }

  www.sendContentAndClear(data);
  www.sendContent("");
}

void processSettingsGet() {
  // from the Settings.htm page -------------------------------------------------------------------
  String v;

  // Slew Speed
  v = www.arg("ss");
  if (!v.equals(EmptyStr)) {
    if (v.equals("vs")) onStep.commandBool(":SX93,5#"); // very slow, 0.5 x
    if (v.equals("s"))  onStep.commandBool(":SX93,4#"); // slow,      0.75x
    if (v.equals("n"))  onStep.commandBool(":SX93,3#"); // normal,    1.0 x
    if (v.equals("f"))  onStep.commandBool(":SX93,2#"); // fast,      1.5 x
    if (v.equals("vf")) onStep.commandBool(":SX93,1#"); // very fast, 2.0 x
  }

  // set-park
  v = www.arg("pk");
  if (!v.equals(EmptyStr)) {
    if (v.equals("s"))    onStep.commandBool(":hQ#");
  }
  // Tracking control
  v = www.arg("tk");
  if (!v.equals(EmptyStr)) {
    if (v.equals("f"))    onStep.commandBlind(":T+#"); // 0.02hz faster
    if (v.equals("-"))    onStep.commandBlind(":T-#"); // 0.02hz slower
    if (v.equals("r"))    onStep.commandBlind(":TR#"); // reset
  }
  // Refraction Rate Tracking control
  v = www.arg("rr");
  if (!v.equals(EmptyStr)) {
    if (v.equals("otk"))  onStep.commandBool(":To#"); // pointing model compensated
    if (v.equals("on"))   onStep.commandBool(":Tr#"); // compensated on
    if (v.equals("off"))  onStep.commandBool(":Tn#"); // compensated off
    if (v.equals("don"))  onStep.commandBool(":T2#"); // compensated 2 axis
    if (v.equals("doff")) onStep.commandBool(":T1#"); // compensated 1 axis
  }
  // Alert buzzer
  v = www.arg("ab");
  if (!v.equals(EmptyStr)) {
    if (v.equals("on"))   onStep.commandBool(":SX97,1#");
    if (v.equals("off"))  onStep.commandBool(":SX97,0#");
  }
  // Auto-continue
  v = www.arg("ma");
  if (!v.equals(EmptyStr)) {
    if (v.equals("now"))  onStep.commandBool(":MN#");
    if (v.equals("on"))   onStep.commandBool(":SX95,1#");
    if (v.equals("off"))  onStep.commandBool(":SX95,0#");
  }
  // Pause at meridian flip
  v = www.arg("mp");
  if (!v.equals(EmptyStr)) {
    if (v.equals("on"))   onStep.commandBool(":SX98,1#");
    if (v.equals("off"))  onStep.commandBool(":SX98,0#");
  }
}
