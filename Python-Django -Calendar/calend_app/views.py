from django.shortcuts import render, redirect
from django.shortcuts import get_object_or_404
from django.views.generic import ListView, DetailView
from .models import Calendar, Day_Event, EventForm
from django.views.generic.edit import CreateView, DeleteView, UpdateView
from django.urls import reverse_lazy
import datetime
import json

class CalendListView(ListView):
    model = Calendar
    template_name = 'home.html'

class NotesListView(ListView):
    model = Day_Event
    template_name = 'notes_list.html'

class NotesListViewFiltered(ListView):
    template_name = 'notes_list.html'

    def get_queryset(self):
        self.day_id = get_object_or_404(Calendar, id=self.kwargs['day_id'])
        return Day_Event.objects.filter(date_id=self.day_id)

    def get_context_data(self, **kwargs):
        context = super().get_context_data(**kwargs)
        context['list_date'] = self.day_id
        return context

    # def form_valid(self, form):
    #     self.day_id = get_object_or_404(Calendar, id=self.kwargs['day_id'])
    #     Day_Event.objects.filter(date_id=self.day_id).delete()
    #     return redirect('home')

class NoteDetailView(DetailView):
    model = Day_Event
    template_name = 'note_detail.html'

# def add_event(request):
#     if request.method == 'POST':
#         formset = EventForm(request.POST)
#         if formset.is_valid():
#             formset.save()
#             return redirect('home')
#     else:
#         formset = EventForm()
#     return render(request, 'add_note.html', {'formset': formset})

    # # Creating a form to add an article.
    # form = ArticleForm()
    #
    # # Creating a form to change an existing article.
    # article = Article.objects.get(pk=1)
    # form = ArticleForm(instance=article)

class NoteCreateView(CreateView):
    model = Day_Event
    fields = ['date', 'text']
    template_name_suffix = '_form_add'

    def get_success_url(self):
        return reverse_lazy('notes_filtered', kwargs={'day_id': self.object.date.id})

class NoteUpdateView(UpdateView):
    model = Day_Event
    fields = ['date', 'text']
    template_name_suffix= '_form'

    def get_success_url(self):
        return reverse_lazy('notes_filtered', kwargs={'day_id': self.object.date.id})

class NoteDeleteView(DeleteView):
    model = Day_Event
    fields = ['date', 'text']

    def get_success_url(self):
        return reverse_lazy('notes_filtered', kwargs={'day_id': self.object.date.id})

def fill_calend(request):
    #json parse
    f = open('calendar_2023.json')
    # returns JSON object as
    # a dictionary
    data = json.load(f)
    months = data['months']
    for m in months:
        month = m['month']
        days = m['days'].replace('*', '').replace('+', '').split(',')
        for day in days:
            date = datetime.date(year=2023, month=int(month), day=int(day))
            # instance = Calendar.objects.get(date=date)
            # instance.type_day = 'Выходной'
            # instance.save()
            instance = Calendar()
            instance.date = date
            instance.type_day = 'Выходной'
            instance.save()
    #
    start = datetime.date(year=2023, month=1, day=1)
    while start.year == 2023:
        try:
            Calendar.objects.get(date=start)
        except Calendar.DoesNotExist:
            instance = Calendar()
            instance.date = start
            instance.type_day = 'Рабочий'
            instance.save()

        start += datetime.timedelta(days=1)

    return redirect('home')