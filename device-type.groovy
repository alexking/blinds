/* 
	Mini Blinds Device Type
	=======================
	
	Based on the SmartThings Arduino device type sample at
	https://gist.github.com/aurman/6862503

*/

metadata {

	tiles {
		
		standardTile("switch", "device.switch", width: 2, height: 2, canChangeIcon: true, canChangeBackground: true) {
			state "on", label: '${name}', action: "switch.off", icon: "st.switches.switch.on", backgroundColor: "#79b821"
			state "off", label: '${name}', action: "switch.on", icon: "st.switches.switch.off", backgroundColor: "#ffffff"
		}
		

		controlTile("level", "device.level", "slider", width: 3, height: 1, canChangeIcon: false, canChangeBackground: false) {
			state "level", action: "switch level.setLevel"
		}     
		
		valueTile("humidity", "device.humidity", width: 1, height: 1) {
			state "default", label:'${currentValue}%'
		}
		
		valueTile("illuminance", "device.illuminance", width: 1, height: 1) {
			 state "default", label: '${currentValue}%',backgroundColors:[
				[value: 0, color: "#000000"],
				[value: 51, color: "#FFFFFF"],
				[value: 60, color: "#FFFFFF"],
				[value: 61, color: "#D5D908"]
			]
		}
		
		standardTile("close", "device.close", width: 1, height: 1, canChangeIcon: true, canChangeBackground: true) {
			state "default", label: 'Set 0', action: "close", icon: "st.contact.contact.closed", backgroundColor: "#ccccff"
		}     

		
		main "switch"
		details(["switch", "humidity", "illuminance", "level", "close"])
	}

}


Map parse(String description) {

	def value = zigbee.parse(description)?.text
	
	// Not super interested in ping, can we just move on? 
	if (value == "ping" || value == " ") 
	{
		return
	}
	
	def linkText = getLinkText(device)
	def descriptionText = getDescriptionText(description, linkText, value)
	def handlerName = value
	def isStateChange = value != "ping"
	def displayed = value && isStateChange
 
	def result = [
		value: value,
		handlerName: handlerName,
		linkText: linkText,
		descriptionText: descriptionText,
		isStateChange: isStateChange,
		displayed: displayed
	]

	if (value in ["!on","!off"])
	{
		result.name  = "switch"
		result.value = value[1..-1]
		
	} else if (value && value[0] == "%") {
		result.name = "level"
		result.value = value[1..-1]
	} else if (value && value[0] == "h") {
		result.name = "humidity";
		result.value = value[1..-1];
		result.unit = "%"
	} else if (value && value[0] == "l") {
		result.name = "illuminance";
		result.value = value[1..-1];
		result.unit = "%"
	
	} else {
		result.name = null; 
	}

 
	if ( (value && value[0] == "%") )
	{
		result.unit = "%"
	
	}
	
	createEvent(name: result.name, value: result.value)
   
	
}
 
def poll() {
	zigbee.smartShield(text: "poll").format()
}

def on() {
	zigbee.smartShield(text: "on").format()
}
 
def off() {
	zigbee.smartShield(text: "off").format()
}

def open() {
	zigbee.smartShield(text: "open").format()
}

def close() {
	zigbee.smartShield(text: "close").format()
}

def setLevel(to)
{
	zigbee.smartShield(text: '%' + to.round() ).format();
}

