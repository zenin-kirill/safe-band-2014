# Прошивка прототипа браслета для мониторинга ПФПЧ

Прошивка прототипа браслета для мониторинга психофизиологического состояния человека, написанная на C под STM32 CortexM3 микроконтроллер. Объект моей научной университетской деятельности как само устройство с датчиками, методами измерений.

* включение/выключение при помощи кнопки, спящий режим
* снятие 3-х сигналов с АПЦ с орпеделенной частотой дискретизации(по таймеру)
* предобработка сигналов
* конфигурация, работа с модулем Bluetooth LE 4.0
* отправка данных сигналов по BLE 4.0
* низкоуровневое управление входными/выходными цифровыми сигналами, для конфигурирования электронных схем
* конфигурация, работа с OLED дисплеем, графический вывод состояния устройства, графиков сигналов, несоклько режимов отображения
* управление зарядкой/питанием от li-ion аккумулятора

**Ссылка на наглядную информацию (документы/ фото):**  
https://drive.google.com/open?id=16R0PIGLphOtBD7Up5n63wx_W4_CKG10b

MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
