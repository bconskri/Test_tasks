from django.db import models
from django.forms import ModelForm

class Calendar(models.Model):
    date = models.DateField(verbose_name= ('Дата'))
    choices = (
        ('Рабочий', 'Рабочий'),
        ('Выходной', 'Выходной'),
    )
    type_day = models.CharField(max_length=15, choices=choices, default='Рабочий', verbose_name= ('Тип'))

    def __str__(self):
        return str(self.date)

    class Meta:
        ordering = ['date']

class Day_Event(models.Model):
    date = models.ForeignKey('Calendar', on_delete=models.CASCADE, null=True, verbose_name= ('Дата'))
    text = models.TextField(verbose_name= ('Текст'))
    time_create = models.DateTimeField(auto_now_add=True, verbose_name= ('Время создания'))
    time_update = models.DateTimeField(auto_now=True, verbose_name= ('Время изменения'))

# Create the form class.
class EventForm(ModelForm):
    class Meta:
        model = Day_Event
        fields = ['date', 'text']

        labels = {
            'date': 'Дата',
            'text': 'Заметка',
        }
