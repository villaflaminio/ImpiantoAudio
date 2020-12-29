import telebot
import ast
import time
from telebot import types
import requests

import json
#tocken del bot telegram
bot = telebot.TeleBot("1471980950:AAHWqlP_UgVQIvNHuRJWa7uIJDG6pk1oLCo")

#codice icone
icoVento = u"\U0001F32C"
frecciaIndietro = u"\U0001F519"
audioOff= u"\U0001F507"
true =u"\u2713"
crossIcon = u"\u274C"

#Queste string vengono associate a dei pulsanti tramite le funzioni make..
stringListLuci = { "luceCaminetto": "luceCaminetto","Verde": "Verde","Rosso": "Rosso","Blu": "Blu","Tutto": "Tutto"}
stringAudio = { "audioFront": "audioFront","audioBack": "audioBack","alimentazione": "sub"}
stringList = {"si": "si", "no": "no"}

#keyboard comando audio
def makeKeyboardAudio():
    markup = types.InlineKeyboardMarkup()

    for key, value in stringAudio.items():
        markup.add(types.InlineKeyboardButton(text=value,callback_data="['value', '" + value + "', '" + key + "']"),
        types.InlineKeyboardButton(text=crossIcon,callback_data="['keyx', '" + key + "']"),
        types.InlineKeyboardButton(text=true, callback_data="['keyv', '" + key + "']"))
    markup.add(types.InlineKeyboardButton(text="ventola", callback_data="['ok', '" + "ok" + "']"),
               types.InlineKeyboardButton(text=crossIcon, callback_data="['ventola', '" + "spegniVentola" + "']"),
               types.InlineKeyboardButton(text=true, callback_data="['ventola', '" + "accendiVentola" + "']"))

    return markup

#keyboard comando luci
def makeKeyboard():
    markup = types.InlineKeyboardMarkup()

    for key, value in stringListLuci.items():
        markup.add(types.InlineKeyboardButton(text=value,callback_data="['value', '" + value + "', '" + key + "']"),
        types.InlineKeyboardButton(text=crossIcon,callback_data="['keyx', '" + key + "']"),
        types.InlineKeyboardButton(text=true, callback_data="['keyv', '" + key + "']"))

    return markup
#pulsanti per l'accensione delle ventole per il raffreddamento
def makeKeyFan():
    markup = types.InlineKeyboardMarkup()
    markup.add(types.InlineKeyboardButton(text="ventola",callback_data="['ok', '" + "ok" + "']" ),
               types.InlineKeyboardButton(text=crossIcon, callback_data="['ventola', '" + "spegniVentola" + "']"),
               types.InlineKeyboardButton(text=true, callback_data="['ventola', '" + "accendiVentola" + "']"))

    return markup


@bot.message_handler(commands=['start'])
def handle_command_adminwindow(message):
    bot.send_message(message.chat.id, "Inserire la password per usare il bot")

@bot.message_handler(regexp="flaminiovilla")
def handle_command_adminwindow(message):
    markup = types.ReplyKeyboardMarkup(row_width=1)
    itembtn1 = types.KeyboardButton('Comanda')
    itembtn2 = types.KeyboardButton('Verifica')
    tutto = types.KeyboardButton('Spegni tutto')

    markup.row(itembtn1, itembtn2)
    markup.row(tutto)

    bot.send_message(message.chat.id, "Ricordati di spegnere quando finisci ", reply_markup=markup)



@bot.message_handler(regexp=frecciaIndietro)
def send_welcome(message):
    markup = types.ReplyKeyboardMarkup(row_width=1)
    itembtn1 = types.KeyboardButton('Comanda')
    itembtn2 = types.KeyboardButton('Verifica')
    tutto = types.KeyboardButton('Spegni tutto')

    markup.row(itembtn1, itembtn2)
    markup.row(tutto)

    bot.send_message(message.chat.id, "Ricordati di spegnere quando finisci ", reply_markup=markup)


@bot.message_handler(regexp="Spegni tutto")
def send_welcome(message):
    r = requests.get('http://192.168.2.89/tuttoOff')
    bot.send_message(message.chat.id, "ALL OFF")


@bot.message_handler(regexp="Comanda")
def send_welcome(message):
    markupSocial = types.ReplyKeyboardMarkup(row_width=2)
    Luci = types.KeyboardButton('Luci')
    Audio = types.KeyboardButton('Audio')
    back = types.InlineKeyboardButton(text=frecciaIndietro, callback_data="['frecciaIndietro', '" + "frecciaIndietro" + "']")
    markupSocial.row(Luci, Audio)
    markupSocial.row(back)
    bot.send_message(message.chat.id, " Modalità di comando", reply_markup=markupSocial)


@bot.message_handler(regexp="Verifica")
def send_welcome(message):
    #eseguendo la richiesta arduino mi restituirà un file json
    #vado a stampare tramite singoli messaggi tutti gli elementi del file

    markupSocial = types.ReplyKeyboardMarkup(row_width=2)
    r = requests.get('http://192.168.2.89/dati')
    richiesta = r.text
    loaded_json = json.loads(richiesta)
    for x in loaded_json:
        dato = ("%s: %s" % (x, loaded_json[x]))
        #if (dato[1] != " off"):
        split= dato.split(": ")
        if (split[1] != "off"):
            bot.send_message(message.chat.id, dato)

#per accendere le luci è necessario avviare l'alimentatore a 12V
#e abilitare 
@bot.message_handler(regexp="Luci")
def send_welcome(message):
    markupSocial = types.ReplyKeyboardMarkup(row_width=2)
    Spegni = types.KeyboardButton('Spegni')
    back = types.InlineKeyboardButton(text=frecciaIndietro, callback_data="['frecciaIndietro', '" + "frecciaIndietro" + "']")
    markupSocial.row(Spegni)
    markupSocial.row(back)
    r = requests.get('http://192.168.2.89/alimLed/on')
    r = requests.get('http://192.168.2.89/alimentazione/on')

    bot.send_message(chat_id=message.chat.id,
                     text="RICORDATI DI SPEGNERE TUTTO QUANDO FINISCI",
                     reply_markup=makeKeyboard(),
                     parse_mode='HTML')
    bot.send_message(message.chat.id, "Controllo luci ", reply_markup=markupSocial)


@bot.message_handler(regexp="Audio")
def send_welcome(message):
    markupAudio = types.ReplyKeyboardMarkup(row_width=2)
    Soft = types.KeyboardButton('Mod Soft')
    Full = types.KeyboardButton('Mod Full')
    ComandiSingoli = types.KeyboardButton('Comandi Singoli')

    SpegniAudio = types.InlineKeyboardButton(text=audioOff, callback_data="['audioOff', '" + "audioOff" + "']")

    back = types.InlineKeyboardButton(text=frecciaIndietro, callback_data="['frecciaIndietro', '" + "frecciaIndietro" + "']")


    markupAudio.row(Soft,Full)
    markupAudio.row(SpegniAudio,back)
    markupAudio.row(ComandiSingoli)

    bot.send_message(chat_id=message.chat.id,
                     text="Audio",
                     reply_markup=makeKeyFan(),
                     parse_mode='HTML')

    bot.send_message(message.chat.id, "Si accende in automatico ", reply_markup=markupAudio)

@bot.message_handler(regexp="Comandi Singoli")
def send_welcome(message):
    markupAudio = types.ReplyKeyboardMarkup(row_width=2)
    SpegniAudio = types.InlineKeyboardButton(text=audioOff, callback_data="['audioOff', '" + "audioOff" + "']")

    back = types.InlineKeyboardButton(text=frecciaIndietro, callback_data="['frecciaIndietro', '" + "frecciaIndietro" + "']")


    markupAudio.row(SpegniAudio,back)

    bot.send_message(chat_id=message.chat.id,
                     text="Audio",
                     reply_markup=makeKeyboardAudio(),
                     parse_mode='HTML')

    bot.send_message(message.chat.id, "Comandi singoli audio", reply_markup=markupAudio)


@bot.message_handler(regexp="Mod Soft")
def send_welcome(message):
    r = requests.get('http://192.168.2.89/modSoft')
    bot.send_message(message.chat.id, "Mod Soft --> Attiva")

@bot.message_handler(regexp="Mod Full")
def send_welcome(message):
    r = requests.get('http://192.168.2.89/modFull')
    bot.send_message(message.chat.id, "Mod Full --> Attiva")

@bot.message_handler(regexp=audioOff)
def send_welcome(message):
    r = requests.get('http://192.168.2.89/tuttoAudioOff')
    bot.send_message(message.chat.id, "Audio --> Spento")

@bot.message_handler(regexp="Spegni")
def send_welcome(message):
    markupSocial = types.ReplyKeyboardMarkup(row_width=2)
    r = requests.get('http://192.168.2.89/verde/off')
    r = requests.get('http://192.168.2.89/rosso/off')
    r = requests.get('http://192.168.2.89/blu/off')
    r = requests.get('http://192.168.2.89/alimLed/off')
    r = requests.get('http://192.168.2.89/alimentazione/off')
    bot.send_message(message.chat.id, " ok fatto ", reply_markup=markupSocial)


@bot.callback_query_handler(func=lambda call: True)
def handle_query(call):

    if (call.data.startswith("['keyv'")):
        #print(f"call.data : {call.data} , type : {type(call.data)}")
        #print(f"ast.literal_eval(call.data) : {ast.literal_eval(call.data)} , type : {type(ast.literal_eval(call.data))}")
        valueFromCallBack = ast.literal_eval(call.data)[1]
        keyFromCallBack = ast.literal_eval(call.data)[1]
        if (keyFromCallBack == "luceCaminetto"):
            r = requests.get('http://192.168.2.89/luceCaminetto/on')
        if (keyFromCallBack == "Verde"):
            r = requests.get('http://192.168.2.89/verde/on')
        if (keyFromCallBack == "Rosso"):
            r = requests.get('http://192.168.2.89/rosso/on')
        if (keyFromCallBack == "Blu"):
            r = requests.get('http://192.168.2.89/blu/on')


        if (keyFromCallBack == "audioFront"):
            r = requests.get('http://192.168.2.89/audioFront/on')
        if (keyFromCallBack == "audioBack"):
            r = requests.get('http://192.168.2.89/audioBack/on')
        if (keyFromCallBack == "alimentazione"):
            r = requests.get('http://192.168.2.89/alimentazione/on')

        if(keyFromCallBack=="Tutto"):
            r = requests.get('http://192.168.2.89/verde/on')
            r = requests.get('http://192.168.2.89/rosso/on')
            r = requests.get('http://192.168.2.89/blu/on')


        bot.answer_callback_query(callback_query_id=call.id,
                              show_alert=True,
                              text="Fatto !")

    if (call.data.startswith("['keyx'")):
        #print(f"call.data : {call.data} , type : {type(call.data)}")
        #print(
        #    f"ast.literal_eval(call.data) : {ast.literal_eval(call.data)} , type : {type(ast.literal_eval(call.data))}")
        valueFromCallBack = ast.literal_eval(call.data)[1]
        keyFromCallBack = ast.literal_eval(call.data)[1]
        if (keyFromCallBack == "luceCaminetto"):
            r = requests.get('http://192.168.2.89/luceCaminetto/off')

        if (keyFromCallBack == "Verde"):
            r = requests.get('http://192.168.2.89/verde/off')
        if (keyFromCallBack == "Rosso"):
            r = requests.get('http://192.168.2.89/rosso/off')
        if (keyFromCallBack == "Blu"):
            r = requests.get('http://192.168.2.89/blu/off')

        if (keyFromCallBack == "audioFront"):
            r = requests.get('http://192.168.2.89/audioFront/off')
        if (keyFromCallBack == "audioBack"):
            r = requests.get('http://192.168.2.89/audioBack/off')
        if (keyFromCallBack == "alimentazione"):
            r = requests.get('http://192.168.2.89/alimentazione/off')

        if (keyFromCallBack == "Tutto"):
            r = requests.get('http://192.168.2.89/verde/off')
            r = requests.get('http://192.168.2.89/rosso/off')
            r = requests.get('http://192.168.2.89/blu/off')

        bot.answer_callback_query(callback_query_id=call.id,
                                  show_alert=True,
                                  text="Fatto !")
    if (call.data.startswith("['ventola'")):
        print(f"call.data : {call.data} , type : {type(call.data)}")
        print(
            f"ast.literal_eval(call.data) : {ast.literal_eval(call.data)} , type : {type(ast.literal_eval(call.data))}")
        valueFromCallBack = ast.literal_eval(call.data)[1]
        keyFromCallBack = ast.literal_eval(call.data)[1]

        if (keyFromCallBack == "accendiVentola"):
            r = requests.get('http://192.168.2.89/ventola/on')

            bot.answer_callback_query(callback_query_id=call.id,
                                      show_alert=True,
                                      text="accesa !")
        if (keyFromCallBack == "spegniVentola"):
            r = requests.get('http://192.168.2.89/ventola/off')

            bot.answer_callback_query(callback_query_id=call.id,
                                      show_alert=True,
                                      text="spenta !")
while True:
    try:
        bot.polling(none_stop=True, interval=0, timeout=0)
    except:
        time.sleep(10)




