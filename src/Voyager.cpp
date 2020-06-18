//
// Created by er713 on 6/15/20.
//

#include "Voyager.h"
#include <cstdio>


Voyager::Voyager(int id, int size) : Logger(id, START) {
    this->size = size;
    rng.seed(time(nullptr));
    i("Zaczynamy");
    //TODO: rozpoczęcie czekania losowego czasu, a potem ubiegania się o kostium
    send_REQUEST_COSTUME(new Message());    // TODO
}

void Voyager::receive_message(Message *msg) {

    timestamp = std::max(timestamp, msg->timestamp) + 1; // aktualizowanie zegaru Lamporta

    switch (state) {
        case START:
            handle_START(msg);
            break;
        case REQUESTING_COSTUME:
            handle_REQUESTING_COSTUME(msg);
            break;
        case HAVE_VESSEL:
            handle_HAVE_VESSEL(msg);
            break;
        case SIGHTSEEING:
            handle_SIGHTSEEING(msg);
            break;
        case WANT_DEPARTURE:
            handle_WANT_DEPARTURE(msg);
            break;
        default:
            handle_REQUESTING_VESSEL(msg);
            break;
    }

}

void Voyager::send_message(Message *msg) {

}

void Voyager::handle_START(Message *msg) {
    auto response = Message(timestamp, id, msg->sender_id);

    switch (msg->msgType) {

        case DEN:
        case REP: //
        case ACK:
        case RES:
            e("To nie powinno sie wydarzyć", msg);
            break;
        case TIC:
            response.msgType = DEN;
            response.send();
            break;
        case OUT: // nie robi nic
            break;
        case REQ: // TODO: opcjonalnie jakieś sprawdzenie, czy zasoby faktycznie są puste
            response.msgType = RES;
            response.data = 0;
            response.send();
            break;
    }

}

void Voyager::handle_REQUESTING_COSTUME(Message *msg) {
    auto *send = new Message(timestamp, id, msg->sender_id); // przepraszam, nie chciałem się wpieprzać od nie mojego kodu, ale mam nadzieję, że nic nie zepsułem (nie mogłem się powstrzymać)
//    send->timestamp = timestamp;
//    send->receiver_id = msg->sender_id;
//    send->sender_id = id;

    switch (msg->msgType) {
        case REQ:
            if (msg->resource == COSTUME && (msg->timestamp > timestamp || (msg->timestamp == timestamp && msg->sender_id > id))) {
                send->msgType = DEN;
            } else {
                send->msgType = RES;
            }
            break;
        case DEN:
            count = 0;
            count_all = 0;
            send_REQUEST_COSTUME(new Message());    // TODO
            break;
        case REP:
            count++;
            count_all++;
            check_VALID_COSTUME();
            break;
        case RES:
            count_all++;
            check_VALID_COSTUME();
            break;
        case TIC:
            send->msgType = DEN;
            break;
        default:
//            printf("To sie nie powinno było wydarzyć (handle_REQUESTING_COSTUME -> default w switch dostał wiadomość: %d)", msg->msgType);
            e("To sie nie powinno było wydarzyć", msg);
            break;
    }
//    send_message(send);
    send->send();
    delete send;
}

void Voyager::check_VALID_COSTUME() {
    if (count_all == size) {
        if (count + 1 < COSTUME_QUANTITY)
            state = static_cast<State>(get_RANDOM_NUMBER(0, VESSEL_QUANTITY - 1));
        count = count_all = 0;
    }
}

void Voyager::handle_HAVE_VESSEL(Message *msg) {
    auto response = Message(timestamp, id, msg->sender_id);

    switch (msg->msgType) {

        case REQ:

            break;
        case DEN:
            break;
        case REP:
            ++count_all;

            break;
        case TIC:
            got_TIC_for = msg->resource;
            break;
        case ACK:
            break;
        case OUT: // wypłynięcie
            state = SIGHTSEEING;
            //TODO: odliczanie czasu msg->data, osobny wątek
            break;
        case RES:
            ++count_all;

            break;
    }

    if (count_all == size - 1) {

    }
}

void Voyager::handle_WANT_DEPARTURE(Message *msg) { //TODO!!!: dodać do sprawozdania, że po TIC odsyłane są DENy kiedy turysta zajmuje więcej miejsa niż jest dostępne, oraz sprawdzanie przed ACK (3.4)

    switch (msg->msgType) {

        case REQ:

            break;
        case DEN:
            ++count_all;
            break;
        case REP:
            break;
        case TIC: // TODO: rozważyć, czy możliwe / czy dwa procesy mogą chcieć wypływać (lepiej żeby było pojedyńczo)
            break;
        case ACK:
            ++count_all;
            get_ACK = true;
            break;
        case OUT: // TODO: rozważyć, czy możliwe / czy dwa procesy mogą chcieć wypływać (lepiej żeby było pojedyńczo)

            break;
        case RES:
            break;
    }

    if (count_all == size - 1) { // kiedy otrzyma wszystkie odpowiedzi

        if (get_ACK) {
            state = HAVE_VESSEL;
        } else {
            auto out = Message(timestamp, id);
            out.msgType = OUT;
            out.data = rng(); //TODO: uzupełnić i sprawdzić
            out.resource;
            out.broadcast(size);
        }

    }
}

void Voyager::handle_SIGHTSEEING(Message *msg) {

}

void Voyager::handle_REQUESTING_VESSEL(Message *msg) {

}

void Voyager::send_REQUEST_COSTUME(Message *msg) {
//    dla każdego odbiorcy (broadcast):
    send_message(msg);
}

int Voyager::get_RANDOM_NUMBER(int a, int b) {
    std::uniform_int_distribution<std::mt19937::result_type> dist6(a, b);
    return dist6(rng);
}

int Voyager::getId() const {
    return id;
}

State Voyager::getState() const {
    return state;
}


