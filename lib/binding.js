const addon = require('../build/Release/universe-native');

function Universe(name) {
    this.greet = function(str) {
        return _addonInstance.greet(str);
    }

    var _addonInstance = new addon.Universe(name);
}

module.exports = Universe;
