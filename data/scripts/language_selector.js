import {ComboBoxController} from "./controllers/combobox_controller.js"
import {Translator} from "./translator.js"

class LanguageSelector extends ComboBoxController{
    _comboBoxItemSelected(theLang){
        Translator.translate(theLang,document);
    }
}

export {LanguageSelector};