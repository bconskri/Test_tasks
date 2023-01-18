from django.urls import path
from .views import CalendListView, NotesListView, NoteDetailView, NotesListViewFiltered, \
    NoteCreateView, NoteUpdateView, NoteDeleteView, fill_calend

urlpatterns = [
    path('', CalendListView.as_view(), name='home'),
    path('date_notes', NotesListView.as_view(), name='notes'),
    path('date_notes/<int:day_id>/', NotesListViewFiltered.as_view(), name='notes_filtered'),
    path('note/<int:pk>/', NoteDetailView.as_view(), name='note_detail'),
    #path('add_note', add_event, name='add_note'),
    path('note/add/', NoteCreateView.as_view(), name='note-add'),
    path('note/update/<int:pk>/', NoteUpdateView.as_view(), name='note-update'),
    path('note/<int:pk>/delete/', NoteDeleteView.as_view(), name='note-delete'),
    path('fill', fill_calend, name='fill-calend'),
]
