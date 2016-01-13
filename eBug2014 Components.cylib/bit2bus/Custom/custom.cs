/*******************************************************************************
* Copyright 2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Text;
using System.Windows.Forms;
using System;

using CyDesigner.Extensions.Common;
using CyDesigner.Extensions.Gde;

namespace bit2bus
{
    public class CyCustomizer :
        ICyShapeCustomize_v1
    {
        #region ICyShapeCustomize_v1 Members

        public CyCustErr CustomizeShapes(
            ICyInstQuery_v1 instQuery,
            ICySymbolShapeEdit_v1 shapeEdit,
            ICyTerminalEdit_v1 termEdit)
        {
            CyCompDevParam  width_param = instQuery.GetCommittedParam("width");
            byte terminalWidth;
            CyCustErr err = width_param.TryGetValueAs<byte>(out terminalWidth);
            if (err.IsNotOk) return err;

			string suffix = (terminalWidth > 1) ? string.Format("[{0}:0]", terminalWidth - 1) : string.Empty;

            string newCntTermName = "bits" + suffix;
            string cntTermName = termEdit.GetTermName("bits");
            
            termEdit.TerminalRename(cntTermName, newCntTermName);
            
            return CyCustErr.OK;
        }

        #endregion
    }
}
//[]//