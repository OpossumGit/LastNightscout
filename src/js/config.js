module.exports = [
  {
    "type": "heading",
    "defaultValue": "LastNightscout Configuration"
  },
  {
    "type": "section",
    "items": [
      {
        "type": "input",
        "messageKey": "URL",
        "label": "Nightscout Address",
        "defaultValue" : "https://nightscout.azurewebsites.net"
      },
      {
        "type": "radiogroup",
        "messageKey": "UNITS",
        "label": "Units Displayed",
        "defaultValue" : "mmol",
        "options": [
          { 
            "label": "mmol/L", 
            "value": "mmol" 
          },
          { 
            "label": "mg/dl", 
            "value": "mg" 
          }
        ]
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];