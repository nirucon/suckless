void tileleft(Monitor *m) {
    unsigned int i, n, h, mw, my, ty;
    Client *c;
    for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next)) n++;
    if (n == 0) return;
    mw = m->ww * (1 - m->mfact);  /* master till HÖGER */
    my = ty = 0;
    for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
        if (i < m->nmaster) { /* master till höger */
            h = (m->wh - my) / (MIN(n, m->nmaster) - i);
            resize(c, m->wx + mw, m->wy + my, m->ww - mw - 2*c->bw, h - 2*c->bw, 0);
            my += HEIGHT(c);
        } else {              /* stack till vänster */
            h = (m->wh - ty) / (n - i);
            resize(c, m->wx, m->wy + ty, mw - 2*c->bw, h - 2*c->bw, 0);
            ty += HEIGHT(c);
        }
    }
}
